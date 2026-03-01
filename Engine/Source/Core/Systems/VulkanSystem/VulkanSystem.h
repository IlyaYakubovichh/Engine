//
// Created by ilya on 01.03.2026.
//

#ifndef ENGINE_VULKANSYSTEM_H
#define ENGINE_VULKANSYSTEM_H

#include "Singleton.h"
#include <vulkan/vulkan.h>

namespace Engine {

    class VulkanSystem final : public Singleton<VulkanSystem> {
        friend class Singleton;
    public:
        [[nodiscard]] VkInstance GetInstance() const { return mInstance; }
        [[nodiscard]] VkDebugUtilsMessengerEXT GetDebugMessenger() const { return mDebugMessenger; }

    private:
        VulkanSystem();
        ~VulkanSystem() override;

        VkInstance mInstance{ VK_NULL_HANDLE };
        VkDebugUtilsMessengerEXT mDebugMessenger{ VK_NULL_HANDLE };
    };

}

#endif //ENGINE_VULKANSYSTEM_H