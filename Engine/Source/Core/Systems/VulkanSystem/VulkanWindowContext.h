//
// Created by ilya on 06.03.2026.
//

#ifndef ENGINE_VULKANWINDOWCONTEXT_H
#define ENGINE_VULKANWINDOWCONTEXT_H

#include "Macro.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {

    struct ENGINE_API VulkanWindowContext {
        VkSurfaceKHR             surface     { VK_NULL_HANDLE };
        VkSwapchainKHR           swapchain   { VK_NULL_HANDLE };
        VkFormat                 imageFormat { VK_FORMAT_UNDEFINED };
        VkExtent2D               extent      {};
        std::vector<VkImage>     images;
        std::vector<VkImageView> imageViews;
    };

}

#endif //ENGINE_VULKANWINDOWCONTEXT_H