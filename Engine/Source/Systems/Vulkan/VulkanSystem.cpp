#include "Vulkan/VulkanSystem.h"
#include "Log/LogSystem.h"
#include "VulkanUtils.h"
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>

namespace Engine {

    class VulkanSystem::Impl {
    public:

        Impl()
        {
            InitInstance();
            InitDeviceWithTempSurface();
            InitSubsystems();
        }

        ~Impl()
        {
            ShutdownSurfaces();
            ShutdownSubsystems();
            ShutdownDevice();
            ShutdownDebugMessenger();
            ShutdownInstance();
        }

        void CreateSurface(uint32_t windowId, const Ref<Window>& window)
        {
            if (mSurfaces.contains(windowId)) {
                ENGINE_LOG_WARN("VulkanSystem", "Surface already exists for id {}", windowId);
                return;
            }

            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VK_CHECK(glfwCreateWindowSurface(mInstance, window->GetRawGLFW(), nullptr, &surface));

            mSurfaces.emplace(windowId, surface);
            ENGINE_LOG_DEBUG("VulkanSystem", "Surface created for window {}", windowId);
        }

        void DestroySurface(uint32_t windowId)
        {
            const auto it = mSurfaces.find(windowId);
            if (it == mSurfaces.end()) {
                ENGINE_LOG_WARN("VulkanSystem", "DestroySurface: id {} not found", windowId);
                return;
            }

            vkDestroySurfaceKHR(mInstance, it->second, nullptr);
            mSurfaces.erase(it);
            ENGINE_LOG_DEBUG("VulkanSystem", "Surface destroyed for window {}", windowId);
        }

        [[nodiscard]] VkSurfaceKHR GetSurface(uint32_t windowId) const
        {
            const auto it = mSurfaces.find(windowId);
            ENGINE_ASSERT_MSG(it != mSurfaces.end(), "VulkanSystem: no surface for given window id");
            return it->second;
        }

        void WaitIdle() const { vkDeviceWaitIdle(mDevice); }

        [[nodiscard]] Ref<VulkanMemAllocSubsystem> GetMemAlloc() const { return GetSubsystem<VulkanMemAllocSubsystem>(); }
        [[nodiscard]] Ref<VulkanSyncSubsystem>     GetSync()     const { return GetSubsystem<VulkanSyncSubsystem>(); }

        [[nodiscard]] VkInstance       GetInstance()           const { return mInstance; }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice()     const { return mPhysicalDevice; }
        [[nodiscard]] VkDevice         GetDevice()             const { return mDevice; }
        [[nodiscard]] VkQueue          GetGraphicsQueue()      const { return mGraphicsQueue; }
        [[nodiscard]] uint32_t         GetGraphicsQueueIndex() const { return mGraphicsQueueIndex; }
        [[nodiscard]] VkQueue          GetPresentQueue()       const { return mPresentationQueue; }
        [[nodiscard]] uint32_t         GetPresentQueueIndex()  const { return mPresentationQueueIndex; }

#ifdef ENGINE_DEBUG
        [[nodiscard]] VkDebugUtilsMessengerEXT GetDebugMessenger() const { return mDebugMessenger; }
#endif

    private:

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
            ENGINE_ASSERT_MSG(result.has_value(), "VulkanSystem: failed to create instance");

            mVkbInstance = result.value();
            mInstance = result->instance;

#ifdef ENGINE_DEBUG
            mDebugMessenger = result->debug_messenger;
#endif
            ENGINE_LOG_DEBUG("VulkanSystem", "Instance created");
        }

        void InitDeviceWithTempSurface()
        {
            VkSurfaceKHR tempSurface = CreateTempSurface();
            InitDevice(tempSurface);
            vkDestroySurfaceKHR(mInstance, tempSurface, nullptr);
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

            ENGINE_ASSERT_MSG(physResult.has_value(), "VulkanSystem: failed to select physical device");
            ENGINE_LOG_INFO("VulkanSystem", "Physical device: {}", physResult->name);

            auto devResult = vkb::DeviceBuilder{ physResult.value() }.build();
            ENGINE_ASSERT_MSG(devResult.has_value(), "VulkanSystem: failed to create logical device");

            const auto gq = devResult->get_queue(vkb::QueueType::graphics);
            const auto gqi = devResult->get_queue_index(vkb::QueueType::graphics);
            const auto pq = devResult->get_queue(vkb::QueueType::present);
            const auto pqi = devResult->get_queue_index(vkb::QueueType::present);

            ENGINE_ASSERT_MSG(gq && gqi && pq && pqi, "VulkanSystem: failed to retrieve device queues");

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
            AddSubsystem<VulkanMemAllocSubsystem>(mInstance, mPhysicalDevice, mDevice);
            AddSubsystem<VulkanSyncSubsystem>();
        }

        [[nodiscard]] VkSurfaceKHR CreateTempSurface()
        {
            glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

            GLFWwindow* tempWindow = glfwCreateWindow(1, 1, "", nullptr, nullptr);
            ENGINE_ASSERT_MSG(tempWindow, "VulkanSystem: failed to create temp window for surface");

            VkSurfaceKHR surface = VK_NULL_HANDLE;
            VK_CHECK(glfwCreateWindowSurface(mInstance, tempWindow, nullptr, &surface));

            glfwDestroyWindow(tempWindow);
            return surface;
        }

        void ShutdownSurfaces()
        {
            for (auto& [id, surface] : mSurfaces)
                vkDestroySurfaceKHR(mInstance, surface, nullptr);
            mSurfaces.clear();
        }

        void ShutdownSubsystems()
        {
            for (auto& [name, subsystem] : mSubsystems)
                subsystem->Shutdown();
            mSubsystems.clear();
        }

        void ShutdownDevice()
        {
            vkDestroyDevice(mDevice, nullptr);
            ENGINE_LOG_DEBUG("VulkanSystem", "Logical device destroyed");
        }

        void ShutdownDebugMessenger()
        {
#ifdef ENGINE_DEBUG
            vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);
            ENGINE_LOG_DEBUG("VulkanSystem", "Debug messenger destroyed");
#endif
        }

        void ShutdownInstance()
        {
            vkDestroyInstance(mInstance, nullptr);
            ENGINE_LOG_DEBUG("VulkanSystem", "Instance destroyed");
        }

        template<std::derived_from<VulkanSubsystem> T, typename... Args>
        void AddSubsystem(Args&&... args)
        {
            mSubsystems.emplace(typeid(T).name(), std::make_shared<T>(std::forward<Args>(args)...));
        }

        template<std::derived_from<VulkanSubsystem> T>
        [[nodiscard]] Ref<T> GetSubsystem() const
        {
            const auto it = mSubsystems.find(typeid(T).name());
            ENGINE_ASSERT_MSG(it != mSubsystems.end(), "VulkanSystem: subsystem not found");
            return std::static_pointer_cast<T>(it->second);
        }

#ifdef ENGINE_DEBUG
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT      severity,
            VkDebugUtilsMessageTypeFlagsEXT             /*type*/,
            const VkDebugUtilsMessengerCallbackDataEXT* data,
            void*                                       /*userData*/)
        {
            switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: ENGINE_LOG_TRACE("Vulkan", "{}", data->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:    ENGINE_LOG_INFO("Vulkan", "{}", data->pMessage);  break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: ENGINE_LOG_WARN("Vulkan", "{}", data->pMessage);  break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:   ENGINE_LOG_ERROR("Vulkan", "{}", data->pMessage); break;
            default:                                              ENGINE_LOG_WARN("Vulkan", "Unknown severity");    break;
            }
            return VK_FALSE;
        }
#endif

        vkb::Instance    mVkbInstance               {};
        VkInstance       mInstance                  { VK_NULL_HANDLE };
        VkPhysicalDevice mPhysicalDevice            { VK_NULL_HANDLE };
        VkDevice         mDevice                    { VK_NULL_HANDLE };
        VkQueue          mGraphicsQueue             { VK_NULL_HANDLE };
        VkQueue          mPresentationQueue         { VK_NULL_HANDLE };
        uint32_t         mGraphicsQueueIndex        { 0 };
        uint32_t         mPresentationQueueIndex    { 0 };

#ifdef ENGINE_DEBUG
        VkDebugUtilsMessengerEXT mDebugMessenger{ VK_NULL_HANDLE };
#endif

        std::unordered_map<uint32_t, VkSurfaceKHR>                mSurfaces;
        std::unordered_map<std::string_view, Ref<VulkanSubsystem>> mSubsystems;
    };

    // ─── Shell ────────────────────────────────────────────────────────────────────

    VulkanSystem::VulkanSystem() : pImpl(std::make_unique<Impl>()) {}
    VulkanSystem::~VulkanSystem() = default;

    void         VulkanSystem::CreateSurface(uint32_t windowId, const Ref<Window>& window) const { pImpl->CreateSurface(windowId, window);  }
    void         VulkanSystem::DestroySurface(uint32_t windowId)                           const { pImpl->DestroySurface(windowId);         }
    void         VulkanSystem::WaitIdle()                                                  const { pImpl->WaitIdle();                       }

    VkSurfaceKHR                 VulkanSystem::GetSurface(uint32_t windowId)  const { return pImpl->GetSurface(windowId);       }
    Ref<VulkanMemAllocSubsystem> VulkanSystem::GetMemAllocSubsystem()         const { return pImpl->GetMemAlloc();              }
    Ref<VulkanSyncSubsystem>     VulkanSystem::GetSyncSubsystem()             const { return pImpl->GetSync();                  }
    VkInstance                   VulkanSystem::GetVkInstance()                const { return pImpl->GetInstance();              }
    VkPhysicalDevice             VulkanSystem::GetVkPhysicalDevice()          const { return pImpl->GetPhysicalDevice();        }
    VkDevice                     VulkanSystem::GetVkDevice()                  const { return pImpl->GetDevice();                }
    VkQueue                      VulkanSystem::GetVkGraphicsQueue()           const { return pImpl->GetGraphicsQueue();         }
    uint32_t                     VulkanSystem::GetVkGraphicsQueueIndex()      const { return pImpl->GetGraphicsQueueIndex();    }
    VkQueue                      VulkanSystem::GetVkPresentQueue()            const { return pImpl->GetPresentQueue();          }
    uint32_t                     VulkanSystem::GetVkPresentQueueIndex()       const { return pImpl->GetPresentQueueIndex();     }

#ifdef ENGINE_DEBUG
    VkDebugUtilsMessengerEXT VulkanSystem::GetDebugMessenger() const { return pImpl->GetDebugMessenger(); }
#endif

} // namespace Engine