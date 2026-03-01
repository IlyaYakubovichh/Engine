//
// Created by ilya on 01.03.2026.
//

#include "VulkanSystem/VulkanSystem.h"
#include "LogSystem/LogSystem.h"
#include <VkBootstrap.h>

namespace Engine {

#ifdef ENGINE_DEBUG
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) {
        switch (messageSeverity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:  ENGINE_LOG_TRACE("Vulkan validation layer", "Message: {}", pCallbackData->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:     ENGINE_LOG_INFO("Vulkan validation layer", "Message: {}", pCallbackData->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:  ENGINE_LOG_WARN("Vulkan validation layer", "Message: {}", pCallbackData->pMessage); break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:    ENGINE_LOG_ERROR("Vulkan validation layer", "Message: {}", pCallbackData->pMessage); break;
            default: ENGINE_LOG_WARN("Vulkan validation layer", "Unknown severity!"); break;
        }

        return VK_FALSE;
    }

    constexpr bool gUseValidationLayers = true;
#else
    constexpr bool gUseValidationLayers = false;
#endif

    VulkanSystem::VulkanSystem() {
        // Vulkan instance (Vulkan context of the Application)
        // Application then can give info about itself to the implementation(driver)
        {
            vkb::InstanceBuilder instanceBuilder;

            const auto vkbInstanceResult = instanceBuilder
            .set_app_name("Engine")
#ifdef ENGINE_DEBUG
            .request_validation_layers(gUseValidationLayers)
            .set_debug_callback(DebugCallback)
#endif
            .require_api_version(VK_API_VERSION_1_3)
            .build();

            if (!vkbInstanceResult) {
                ENGINE_LOG_ERROR("VulkanSystem", "Failed to create vkbInstance: {}!", vkbInstanceResult.error().message());
                return;
            }

            auto vkbInstance = vkbInstanceResult.value();
            mInstance = vkbInstance.instance;
            mDebugMessenger = vkbInstance.debug_messenger;
        }
    }

    VulkanSystem::~VulkanSystem() {
        // Destroy debug messenger
        {
            vkb::destroy_debug_utils_messenger(mInstance, mDebugMessenger);
        }

        // Destroy instance
        {
           vkDestroyInstance(mInstance, nullptr);
        }
    }

}