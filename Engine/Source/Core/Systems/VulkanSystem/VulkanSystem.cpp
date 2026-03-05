#include "VulkanSystem/VulkanSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"

#include <GLFW/glfw3.h>
#include <VkBootstrap.h>

namespace Engine {

// ---------------------------------------------------------------------------
// PIMPL
// ---------------------------------------------------------------------------
    class VulkanSystem::Impl {
    public:
        Impl()  {
            InitInstance();
            InitSurface();
            InitDevice();
        }

        ~Impl() {
            vkDestroyDevice(mDevice, nullptr);
            ENGINE_LOG_INFO("VulkanSystem", "Logical device destroyed");

            vkDestroySurfaceKHR(mInstance, mSurface, nullptr);
            ENGINE_LOG_INFO("VulkanSystem", "Surface destroyed");

#ifdef ENGINE_DEBUG
            vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);
            ENGINE_LOG_INFO("VulkanSystem", "Debug messenger destroyed");
#endif

            vkDestroyInstance(mInstance, nullptr);
            ENGINE_LOG_INFO("VulkanSystem", "Instance destroyed");
        }

        // Getters
        [[nodiscard]] VkInstance       GetInstance()           const { return mInstance;           }
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice()     const { return mPhysicalDevice;     }
        [[nodiscard]] VkDevice         GetDevice()             const { return mDevice;              }
        [[nodiscard]] VkSurfaceKHR     GetSurface()            const { return mSurface;             }
        [[nodiscard]] VkQueue          GetGraphicsQueue()      const { return mGraphicsQueue;       }
        [[nodiscard]] uint32_t         GetGraphicsQueueIndex() const { return mGraphicsQueueIndex;  }

#ifdef ENGINE_DEBUG
        [[nodiscard]] VkDebugUtilsMessengerEXT GetDebugMessenger() const { return mDebugMessenger; }
#endif

    private:
// ---------------------------------------------------------------------------
// Debug callback
// ---------------------------------------------------------------------------
#ifdef ENGINE_DEBUG
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT             /*messageType*/,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
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
            ENGINE_LOG_INFO("VulkanSystem", "Instance created successfully");
        }

        void InitSurface() {
            // VkBoostrap handles WSI under the hood
            const auto window = WindowSystem::GetInstance()->GetWindowById(gMainWindowId);
            VULKAN_CHECK(glfwCreateWindowSurface(mInstance, window->GetRawGLFW(), nullptr, &mSurface));
            ENGINE_LOG_INFO("VulkanSystem", "Surface created successfully");
        }

        void InitDevice() {
            // Physical device
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

            auto physResult = vkb::PhysicalDeviceSelector{ mVkbInstance }
                .set_minimum_version(1, 3)
                .set_required_features_13(features13)
                .set_required_features_12(features12)
                .set_surface(mSurface)
                .select();

            if (!physResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to select physical device: {}", physResult.error().message());
                return;
            }

            ENGINE_LOG_INFO("VulkanSystem", "Physical device selected: {}", physResult->name);

            // Logical device
            auto devResult = vkb::DeviceBuilder{ physResult.value() }.build();
            if (!devResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to create logical device: {}", devResult.error().message());
                return;
            }

            // Graphics queue
            auto queueResult = devResult->get_queue(vkb::QueueType::graphics);
            auto indexResult = devResult->get_queue_index(vkb::QueueType::graphics);
            if (!queueResult || !indexResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to get graphics queue");
                return;
            }

            mPhysicalDevice    = physResult->physical_device;
            mDevice            = devResult->device;
            mGraphicsQueue     = queueResult.value();
            mGraphicsQueueIndex = indexResult.value();

            ENGINE_LOG_INFO("VulkanSystem", "Logical device created successfully");
            ENGINE_LOG_INFO("VulkanSystem", "Graphics queue index: {}", mGraphicsQueueIndex);
        }

// ---------------------------------------------------------------------------
// Handles
// ---------------------------------------------------------------------------
        vkb::Instance mVkbInstance          {};

        VkInstance               mInstance           { VK_NULL_HANDLE };
        VkSurfaceKHR             mSurface            { VK_NULL_HANDLE };
        VkPhysicalDevice         mPhysicalDevice     { VK_NULL_HANDLE };
        VkDevice                 mDevice             { VK_NULL_HANDLE };
        VkQueue                  mGraphicsQueue      { VK_NULL_HANDLE };
        uint32_t                 mGraphicsQueueIndex { 0 };

#ifdef ENGINE_DEBUG
        VkDebugUtilsMessengerEXT mDebugMessenger     { VK_NULL_HANDLE };
#endif
    };

// ---------------------------------------------------------------------------
// VulkanSystem - thin shell, delegates everything to Impl
// ---------------------------------------------------------------------------
    VulkanSystem::VulkanSystem()
        : pImpl(std::make_unique<Impl>()) {}

    VulkanSystem::~VulkanSystem() = default;

    VkInstance       VulkanSystem::GetInstance()           const { return pImpl->GetInstance();           }
    VkPhysicalDevice VulkanSystem::GetPhysicalDevice()     const { return pImpl->GetPhysicalDevice();     }
    VkDevice         VulkanSystem::GetDevice()             const { return pImpl->GetDevice();              }
    VkSurfaceKHR     VulkanSystem::GetSurface()            const { return pImpl->GetSurface();             }
    VkQueue          VulkanSystem::GetGraphicsQueue()      const { return pImpl->GetGraphicsQueue();       }
    uint32_t         VulkanSystem::GetGraphicsQueueIndex() const { return pImpl->GetGraphicsQueueIndex();  }

#ifdef ENGINE_DEBUG
    VkDebugUtilsMessengerEXT VulkanSystem::GetDebugMessenger() const { return pImpl->GetDebugMessenger(); }
#endif

} // namespace Engine