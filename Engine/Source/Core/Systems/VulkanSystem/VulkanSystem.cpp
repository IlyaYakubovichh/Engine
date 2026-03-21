#include "VulkanSystem.h"
#include "Systems/LogSystem/LogSystem.h"
#include "VulkanUtils.h"
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>
#include <ranges>
#include <typeinfo>
#include <vector>
#include <unordered_map>

namespace Engine {

    // ─── PIMPL ───────────────────────────────────────────────────────────────────

    class VulkanSystem::Impl {
    public:
        Impl()
        {
            InitInstance();

            // Temporary surface — used only for physical device selection.
            VkSurfaceKHR initSurface = InitSurface();
            InitDevice(initSurface);
            vkDestroySurfaceKHR(mInstance, initSurface, nullptr);
            ENGINE_LOG_DEBUG("VulkanSystem", "Init surface destroyed");

            InitSubsystems();
        }

        ~Impl()
        {
            DestroyAllWindowContexts();
            ShutdownSubsystems();
            DestroyDevice();
            DestroyDebugMessenger();
            DestroyInstance();
        }

        // ── Window contexts ───────────────────────────────────────────────────────

        void CreateWindowContext(uint32_t windowId, const Ref<Window>& window)
        {
            // Create surface only if context does not exist yet.
            auto it = mWindowContexts.find(windowId);
            if (it == mWindowContexts.end()) {
                VulkanWindowContext ctx;
                VK_CHECK(glfwCreateWindowSurface(mInstance, window->GetRawGLFW(), nullptr, &ctx.surface));
                ENGINE_LOG_DEBUG("VulkanSystem", "Surface created for window {}", windowId);
                it = mWindowContexts.emplace(windowId, std::move(ctx)).first;
            }

            auto [width, height] = window->GetExtent();
            auto& [surface, swapchain] = it->second;

            swapchain = std::make_shared<VulkanSwapchain>();
            swapchain->Initialize(VulkanSwapchainSpec{
                .physicalDevice = mPhysicalDevice,
                .device = mDevice,
                .surface = surface,
                .width = static_cast<uint32_t>(width),
                .height = static_cast<uint32_t>(height),
                });

            // TODO (multi-window): allocate per-window render target (color + depth image)
            // so each window gets its own draw surface for off-screen rendering.
            //   mRenderTargets[windowId] = CreateRenderTarget(width, height);
        }

        void DestroyWindowContext(uint32_t windowId)
        {
            const auto it = mWindowContexts.find(windowId);
            if (it == mWindowContexts.end()) {
                ENGINE_LOG_WARN("VulkanSystem", "DestroyWindowContext: id {} not found", windowId);
                return;
            }

            auto& [surface, swapchain] = it->second;
            if (swapchain) swapchain->Destroy(mDevice);
            vkDestroySurfaceKHR(mInstance, surface, nullptr);

            // TODO (multi-window): free the per-window render target here.
            //   mRenderTargets.erase(windowId);

            ENGINE_LOG_DEBUG("VulkanSystem", "WindowContext destroyed for window {}", windowId);
            mWindowContexts.erase(it);
        }

        [[nodiscard]] const VulkanWindowContext& GetWindowContext(uint32_t windowId) const
        {
            const auto it = mWindowContexts.find(windowId);
            ENGINE_ASSERT_MSG(it != mWindowContexts.end(),
                "VulkanSystem: no context for given window id");
            return it->second;
        }

        // ── Subsystem accessors ───────────────────────────────────────────────────

        [[nodiscard]] Ref<VulkanMemAllocSubsystem> GetMemAllocSubsystem() const
        {
            return GetSubsystem<VulkanMemAllocSubsystem>();
        }

        [[nodiscard]] Ref<VulkanSyncSubsystem> GetSyncSubsystem() const
        {
            return GetSubsystem<VulkanSyncSubsystem>();
        }

        // ── Device / queue accessors ──────────────────────────────────────────────

        void WaitDeviceIdle() const { vkDeviceWaitIdle(mDevice); }

        [[nodiscard]] VkInstance       GetInstance()               const { return mInstance;                }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice()         const { return mPhysicalDevice;          }
        [[nodiscard]] VkDevice         GetDevice()                 const { return mDevice;                  }
        [[nodiscard]] VkQueue          GetGraphicsQueue()          const { return mGraphicsQueue;           }
        [[nodiscard]] uint32_t         GetGraphicsQueueIndex()     const { return mGraphicsQueueIndex;      }
        [[nodiscard]] VkQueue          GetPresentationQueue()      const { return mPresentationQueue;       }
        [[nodiscard]] uint32_t         GetPresentationQueueIndex() const { return mPresentationQueueIndex;  }

#ifdef ENGINE_DEBUG
        [[nodiscard]] VkDebugUtilsMessengerEXT GetDebugMessenger() const { return mDebugMessenger; }
#endif

    private:
        // ── Shutdown helpers ──────────────────────────────────────────────────────

        // Destroys all window contexts (swapchains depend on the device).
        void DestroyAllWindowContexts()
        {
            // Collect ids first — DestroyWindowContext modifies the map.
            const auto ids = mWindowContexts
                | std::views::keys
                | std::ranges::to<std::vector>();

            for (const auto id : ids) {
                DestroyWindowContext(id);
            }
        }

        // Shuts down all subsystems in reverse registration order.
        void ShutdownSubsystems()
        {
            for (auto& [name, subsystem] : std::ranges::reverse_view(mSubsystems)) {
                subsystem->Destroy();
            }
            mSubsystems.clear();
        }

        void DestroyDevice()
        {
            vkDestroyDevice(mDevice, nullptr);
            ENGINE_LOG_DEBUG("VulkanSystem", "Logical device destroyed");
        }

        void DestroyDebugMessenger()
        {
#ifdef ENGINE_DEBUG
            vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);
            ENGINE_LOG_DEBUG("VulkanSystem", "Debug messenger destroyed");
#endif
        }

        void DestroyInstance()
        {
            vkDestroyInstance(mInstance, nullptr);
            ENGINE_LOG_DEBUG("VulkanSystem", "Instance destroyed");
        }

        // ── Subsystem registration ────────────────────────────────────────────────

        // Registers a subsystem by its type name and returns it.
        template<std::derived_from<VulkanSubsystem> T>
        Ref<T> AddSubsystem(Ref<T> subsystem)
        {
            mSubsystems.emplace_back(typeid(T).name(), subsystem);
            return subsystem;
        }

        // Returns a subsystem by type. Asserts if not found.
        template<std::derived_from<VulkanSubsystem> T>
        [[nodiscard]] Ref<T> GetSubsystem() const
        {
            const std::string_view key = typeid(T).name();
            for (const auto& [name, subsystem] : mSubsystems) {
                if (name == key) return std::static_pointer_cast<T>(subsystem);
            }
            ENGINE_ASSERT_MSG(false, "VulkanSystem: subsystem not found");
            return nullptr;
        }

        // ── Initialisation helpers ────────────────────────────────────────────────

        void InitInstance()
        {
            vkb::InstanceBuilder builder;
            builder
                .set_app_name("Engine")
                .require_api_version(VK_API_VERSION_1_3);

#ifdef ENGINE_DEBUG
            builder
                .request_validation_layers(true)
                .set_debug_callback(DebugCallback);
#endif

            auto result = builder.build();
            if (!result) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to create instance: {}",
                    result.error().message());
                return;
            }

            mVkbInstance = result.value();
            mInstance = result->instance;

#ifdef ENGINE_DEBUG
            mDebugMessenger = result->debug_messenger;
#endif

            ENGINE_LOG_DEBUG("VulkanSystem", "Instance created");
        }

        // Creates a temporary 1x1 hidden window, extracts a VkSurfaceKHR,
        // then immediately destroys the window.
        [[nodiscard]] VkSurfaceKHR InitSurface()
        {
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            GLFWwindow* tempWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);
            ENGINE_ASSERT_MSG(tempWindow != nullptr, "VulkanSystem: failed to create init window");

            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VK_CHECK(glfwCreateWindowSurface(mInstance, tempWindow, nullptr, &surface));

            glfwDestroyWindow(tempWindow);

            ENGINE_LOG_DEBUG("VulkanSystem", "Init surface created (temp window destroyed)");
            return surface;
        }

        void InitDevice(VkSurfaceKHR surface)
        {
            const VkPhysicalDeviceVulkan13Features features13{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
                .synchronization2 = VK_TRUE,
                .dynamicRendering = VK_TRUE,
            };

            const VkPhysicalDeviceVulkan12Features features12{
                .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                .descriptorIndexing = VK_TRUE,
                .bufferDeviceAddress = VK_TRUE,
            };

            auto physResult = vkb::PhysicalDeviceSelector{ mVkbInstance }
                .set_minimum_version(1, 3)
                .set_required_features_13(features13)
                .set_required_features_12(features12)
                .set_surface(surface)
                .select();

            if (!physResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to select physical device: {}", physResult.error().message());
                return;
            }

            ENGINE_LOG_INFO("VulkanSystem", "Physical device: {}", physResult->name);

            auto devResult = vkb::DeviceBuilder{ physResult.value() }.build();
            if (!devResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to create logical device: {}", devResult.error().message());
                return;
            }

            const auto gq = devResult->get_queue(vkb::QueueType::graphics);
            const auto gqi = devResult->get_queue_index(vkb::QueueType::graphics);
            const auto pq = devResult->get_queue(vkb::QueueType::present);
            const auto pqi = devResult->get_queue_index(vkb::QueueType::present);

            if (!gq || !gqi || !pq || !pqi) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to retrieve device queues");
                return;
            }

            mPhysicalDevice = physResult->physical_device;
            mDevice = devResult->device;
            mGraphicsQueue = gq.value();
            mGraphicsQueueIndex = gqi.value();
            mPresentationQueue = pq.value();
            mPresentationQueueIndex = pqi.value();

            ENGINE_LOG_DEBUG("VulkanSystem", "Logical device created");
            ENGINE_LOG_DEBUG("VulkanSystem", "Graphics queue index: {}", mGraphicsQueueIndex);
            ENGINE_LOG_DEBUG("VulkanSystem", "Present  queue index: {}", mPresentationQueueIndex);
        }

        void InitSubsystems()
        {
            AddSubsystem(std::make_shared<VulkanMemAllocSubsystem>(mInstance, mPhysicalDevice, mDevice));
            AddSubsystem(std::make_shared<VulkanSyncSubsystem>());
        }

        // ── Debug callback ────────────────────────────────────────────────────────

#ifdef ENGINE_DEBUG
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
            VkDebugUtilsMessageTypeFlagsEXT             /*type*/,
            const VkDebugUtilsMessengerCallbackDataEXT* data,
            void*                                       /*userData*/)
        {
            switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                ENGINE_LOG_TRACE("Vulkan", "{}", data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                ENGINE_LOG_INFO("Vulkan", "{}", data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                ENGINE_LOG_WARN("Vulkan", "{}", data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                ENGINE_LOG_ERROR("Vulkan", "{}", data->pMessage); break;
            default:
                ENGINE_LOG_WARN("Vulkan", "Unknown severity");   break;
            }
            return VK_FALSE;
        }
#endif

        // ── Members ───────────────────────────────────────────────────────────────

        vkb::Instance    mVkbInstance{};

        VkInstance       mInstance              { VK_NULL_HANDLE };
        VkPhysicalDevice mPhysicalDevice        { VK_NULL_HANDLE };
        VkDevice         mDevice                { VK_NULL_HANDLE };
        VkQueue          mGraphicsQueue         { VK_NULL_HANDLE };
        VkQueue          mPresentationQueue     { VK_NULL_HANDLE };
        uint32_t         mGraphicsQueueIndex    { 0 };
        uint32_t         mPresentationQueueIndex{ 0 };

#ifdef ENGINE_DEBUG
        VkDebugUtilsMessengerEXT mDebugMessenger{ VK_NULL_HANDLE };
#endif

        std::unordered_map<uint32_t, VulkanWindowContext> mWindowContexts;

        // TODO (multi-window): add per-window render targets map here.
        // std::unordered_map<uint32_t, VulkanRenderTarget> mRenderTargets;

        // Subsystems in registration order (used for reverse shutdown).
        std::vector<std::pair<std::string_view, Ref<VulkanSubsystem>>> mSubsystems;
    };

    // ─── VulkanSystem shell ───────────────────────────────────────────────────────

    VulkanSystem::VulkanSystem() : pImpl(std::make_unique<Impl>()) {}
    VulkanSystem::~VulkanSystem() = default;

    void VulkanSystem::CreateVulkanWindowContext(uint32_t windowId, const Ref<Window>& window) const
    {
        pImpl->CreateWindowContext(windowId, window);
    }

    void VulkanSystem::DestroyVulkanWindowContext(uint32_t windowId) const
    {
        pImpl->DestroyWindowContext(windowId);
    }

    const VulkanWindowContext& VulkanSystem::GetWindowContext(uint32_t windowId) const
    {
        return pImpl->GetWindowContext(windowId);
    }

    Ref<VulkanMemAllocSubsystem> VulkanSystem::GetMemAllocSubsystem() const { return pImpl->GetMemAllocSubsystem();     }
    Ref<VulkanSyncSubsystem>     VulkanSystem::GetSyncSubsystem()     const { return pImpl->GetSyncSubsystem();         }
    void                         VulkanSystem::WaitDeviceIdle()       const { pImpl->WaitDeviceIdle();                  }

    VkInstance       VulkanSystem::GetVkInstance()               const { return pImpl->GetInstance();                   }
    VkPhysicalDevice VulkanSystem::GetVkPhysicalDevice()         const { return pImpl->GetPhysicalDevice();             }
    VkDevice         VulkanSystem::GetVkDevice()                 const { return pImpl->GetDevice();                     }
    VkQueue          VulkanSystem::GetVkGraphicsQueue()          const { return pImpl->GetGraphicsQueue();              }
    uint32_t         VulkanSystem::GetVkGraphicsQueueIndex()     const { return pImpl->GetGraphicsQueueIndex();         }
    VkQueue          VulkanSystem::GetVkPresentationQueue()      const { return pImpl->GetPresentationQueue();          }
    uint32_t         VulkanSystem::GetVkPresentationQueueIndex() const { return pImpl->GetPresentationQueueIndex();     }

#ifdef ENGINE_DEBUG
    VkDebugUtilsMessengerEXT VulkanSystem::GetDebugMessenger() const { return pImpl->GetDebugMessenger(); }
#endif

} // namespace Engine