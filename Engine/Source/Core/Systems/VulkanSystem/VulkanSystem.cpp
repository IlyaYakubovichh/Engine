#include "VulkanSystem/VulkanSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"
#include <VkBootstrap.h>
#include <GLFW/glfw3.h>

namespace Engine {

// ---------------------------------------------------------------------------
// PIMPL
// ---------------------------------------------------------------------------
    class VulkanSystem::Impl {
    public:
        Impl() {
            InitInstance();
            InitMainSurface();
            InitDevice();
        }

        ~Impl() {
            // Vulkan window context
            std::vector<uint32_t> ids;
            ids.reserve(mWindowContexts.size());
            for (const auto& id : mWindowContexts | std::views::keys) ids.push_back(id);
            for (const auto id : ids) DestroyWindowContext(id);

            // Device
            vkDestroyDevice(mDevice, nullptr);
            ENGINE_LOG_DEBUG("VulkanSystem", "Logical device destroyed");

            // Debug messenger
#ifdef ENGINE_DEBUG
            vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);
            ENGINE_LOG_DEBUG("VulkanSystem", "Debug messenger destroyed");
#endif

            // Instance
            vkDestroyInstance(mInstance, nullptr);
            ENGINE_LOG_DEBUG("VulkanSystem", "Instance destroyed");
        }

        // -----------------------------------------------------------------------
        // Per-window context
        // -----------------------------------------------------------------------
        void CreateWindowContext(const uint32_t windowId, const std::shared_ptr<Window>& window) {
            auto it = mWindowContexts.find(windowId);
            if (it == mWindowContexts.end()) {
                VulkanWindowContext ctx;
                VULKAN_CHECK(glfwCreateWindowSurface(mInstance, window->GetRawGLFW(), nullptr, &ctx.surface));
                ENGINE_LOG_DEBUG("VulkanSystem", "Surface created for window {}", windowId);
                it = mWindowContexts.emplace(windowId, std::move(ctx)).first;
            }

            auto [width, height] = window->GetExtent();
            auto&[surface, swapchain] = it->second;

            swapchain = std::make_shared<VulkanSwapchain>();
            swapchain->Initialize(VulkanSwapchainSpecification {
                .physicalDevice = mPhysicalDevice,
                .device         = mDevice,
                .surface        = surface,
                .width          = static_cast<uint32_t>(width),
                .height         = static_cast<uint32_t>(height),
            });
        }

        void DestroyWindowContext(const uint32_t windowId) {
            const auto it = mWindowContexts.find(windowId);
            if (it == mWindowContexts.end()) {
                ENGINE_LOG_WARN("VulkanSystem", "No context found for window {}", windowId);
                return;
            }

            auto&[surface, swapchain] = it->second;

            if (swapchain) {
                swapchain->Destroy(mDevice);
            }

            vkDestroySurfaceKHR(mInstance, surface, nullptr);

            ENGINE_LOG_DEBUG("VulkanSystem", "VulkanWindowContext destroyed for window {}", windowId);
            mWindowContexts.erase(it);
        }

        [[nodiscard]] const VulkanWindowContext& GetWindowContext(const uint32_t windowId) const {
            const auto it = mWindowContexts.find(windowId);
            ENGINE_ASSERT_MESSAGE(it != mWindowContexts.end(), "No VulkanWindowContext for window with given id");
            return it->second;
        }

        // Getters
        [[nodiscard]] VkInstance       GetInstance()               const { return mInstance;               }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice()         const { return mPhysicalDevice;         }
        [[nodiscard]] VkDevice         GetDevice()                 const { return mDevice;                 }
        [[nodiscard]] VkQueue          GetGraphicsQueue()          const { return mGraphicsQueue;          }
        [[nodiscard]] uint32_t         GetGraphicsQueueIndex()     const { return mGraphicsQueueIndex;     }
        [[nodiscard]] VkQueue          GetPresentationQueue()      const { return mPresentationQueue;      }
        [[nodiscard]] uint32_t         GetPresentationQueueIndex() const { return mPresentationQueueIndex; }

#ifdef ENGINE_DEBUG
        [[nodiscard]] VkDebugUtilsMessengerEXT GetDebugMessenger() const { return mDebugMessenger; }
#endif

    private:
// ---------------------------------------------------------------------------
// Debug callback
// ---------------------------------------------------------------------------
#ifdef ENGINE_DEBUG
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT             /*messageType*/,
            const VkDebugUtilsMessengerCallbackDataEXT*  pCallbackData,
            void*                                       /*pUserData*/)
        {
            switch (messageSeverity) {
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
                    ENGINE_LOG_TRACE("Vulkan", "{}", pCallbackData->pMessage); break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                    ENGINE_LOG_INFO ("Vulkan", "{}", pCallbackData->pMessage); break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                    ENGINE_LOG_WARN ("Vulkan", "{}", pCallbackData->pMessage); break;
                case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                    ENGINE_LOG_ERROR("Vulkan", "{}", pCallbackData->pMessage); break;
                default:
                    ENGINE_LOG_WARN("Vulkan", "Unknown severity!"); break;
            }
            return VK_FALSE;
        }
#endif

// ---------------------------------------------------------------------------
// Init helpers
// ---------------------------------------------------------------------------
        void InitInstance() {
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
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to create instance: {}", result.error().message());
                return;
            }

            mVkbInstance = result.value();
            mInstance    = result->instance;
#ifdef ENGINE_DEBUG
            mDebugMessenger = result->debug_messenger;
#endif
            ENGINE_LOG_DEBUG("VulkanSystem", "Instance created successfully");
        }

        void InitMainSurface() {
            const auto window = WindowSystem::GetInstance()->GetWindowById(gMainWindowId);

            VulkanWindowContext ctx;
            VULKAN_CHECK(glfwCreateWindowSurface(mInstance, window->GetRawGLFW(), nullptr, &ctx.surface));
            ENGINE_LOG_DEBUG("VulkanSystem", "Surface created for main window ({})", gMainWindowId);

            mWindowContexts.emplace(gMainWindowId, std::move(ctx));
        }

        void InitDevice() {
            VkPhysicalDeviceVulkan13Features features13 {
                .sType            = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
                .synchronization2 = VK_TRUE,
                .dynamicRendering = VK_TRUE,
            };

            VkPhysicalDeviceVulkan12Features features12 {
                .sType               = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
                .descriptorIndexing  = VK_TRUE,
                .bufferDeviceAddress = VK_TRUE,
            };

            VkSurfaceKHR mainSurface = mWindowContexts.at(gMainWindowId).surface;

            auto physResult = vkb::PhysicalDeviceSelector{ mVkbInstance }
                .set_minimum_version(1, 3)
                .set_required_features_13(features13)
                .set_required_features_12(features12)
                .set_surface(mainSurface)
                .select();

            if (!physResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to select physical device: {}", physResult.error().message());
                return;
            }

            ENGINE_LOG_INFO("VulkanSystem", "Physical device selected: {}", physResult->name);

            auto devResult = vkb::DeviceBuilder{ physResult.value() }.build();
            if (!devResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to create logical device: {}", devResult.error().message());
                return;
            }

            auto gq  = devResult->get_queue(vkb::QueueType::graphics);
            auto gqi  = devResult->get_queue_index(vkb::QueueType::graphics);
            auto pq  = devResult->get_queue(vkb::QueueType::present);
            auto pqi  = devResult->get_queue_index(vkb::QueueType::present);

            if (!gq || !gqi || !pq || !pqi) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to get queues");
                return;
            }

            mPhysicalDevice         = physResult->physical_device;
            mDevice                 = devResult->device;
            mGraphicsQueue          = gq.value();
            mGraphicsQueueIndex     = gqi.value();
            mPresentationQueue      = pq.value();
            mPresentationQueueIndex = pqi.value();

            ENGINE_LOG_DEBUG("VulkanSystem", "Logical device created successfully");
            ENGINE_LOG_DEBUG("VulkanSystem", "Graphics queue index: {}", mGraphicsQueueIndex);
            ENGINE_LOG_DEBUG("VulkanSystem", "Present queue index:  {}", mPresentationQueueIndex);
        }

// ---------------------------------------------------------------------------
// Handles
// ---------------------------------------------------------------------------
        vkb::Instance mVkbInstance {};

        VkInstance       mInstance           { VK_NULL_HANDLE };
        VkPhysicalDevice mPhysicalDevice     { VK_NULL_HANDLE };
        VkDevice         mDevice             { VK_NULL_HANDLE };
        VkQueue          mGraphicsQueue      { VK_NULL_HANDLE };
        VkQueue          mPresentationQueue  { VK_NULL_HANDLE };
        uint32_t         mGraphicsQueueIndex     { 0 };
        uint32_t         mPresentationQueueIndex { 0 };

#ifdef ENGINE_DEBUG
        VkDebugUtilsMessengerEXT mDebugMessenger { VK_NULL_HANDLE };
#endif

        std::unordered_map<uint32_t, VulkanWindowContext> mWindowContexts;
    };

// ---------------------------------------------------------------------------
// VulkanSystem - thin shell, delegates everything to Impl
// ---------------------------------------------------------------------------
    VulkanSystem::VulkanSystem()
        : pImpl(std::make_unique<Impl>()) {}

    VulkanSystem::~VulkanSystem() = default;

    void VulkanSystem::CreateVulkanWindowContext(const uint32_t windowId, const std::shared_ptr<Window>& window) const {
        pImpl->CreateWindowContext(windowId, window);
    }

    void VulkanSystem::DestroyVulkanWindowContext(const uint32_t windowId) const {
        pImpl->DestroyWindowContext(windowId);
    }

    const VulkanWindowContext& VulkanSystem::GetWindowContext(const uint32_t windowId) const {
        return pImpl->GetWindowContext(windowId);
    }

    VkInstance       VulkanSystem::GetVkInstance()               const { return pImpl->GetInstance();               }
    VkPhysicalDevice VulkanSystem::GetVkPhysicalDevice()         const { return pImpl->GetPhysicalDevice();         }
    VkDevice         VulkanSystem::GetVkDevice()                 const { return pImpl->GetDevice();                 }
    VkQueue          VulkanSystem::GetVkGraphicsQueue()          const { return pImpl->GetGraphicsQueue();          }
    uint32_t         VulkanSystem::GetVkGraphicsQueueIndex()     const { return pImpl->GetGraphicsQueueIndex();     }
    VkQueue          VulkanSystem::GetVkPresentationQueue()      const { return pImpl->GetPresentationQueue();      }
    uint32_t         VulkanSystem::GetVkPresentationQueueIndex() const { return pImpl->GetPresentationQueueIndex(); }

#ifdef ENGINE_DEBUG
    VkDebugUtilsMessengerEXT VulkanSystem::GetDebugMessenger() const { return pImpl->GetDebugMessenger(); }
#endif

} // namespace Engine