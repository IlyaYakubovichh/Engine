//
// Created by ilya on 06.03.2026.
//

#ifndef ENGINE_VULKANWINDOWCONTEXT_H
#define ENGINE_VULKANWINDOWCONTEXT_H

#include "Macro.h"
#include "Utility.h"
#include "VulkanSystem/VulkanSwapchain.h"
#include <memory>
#include <vulkan/vulkan.h>

namespace Engine {

    struct ENGINE_API VulkanWindowContext {
        VkSurfaceKHR         surface   { VK_NULL_HANDLE };
        Ref<VulkanSwapchain> swapchain;
    };

}

#endif //ENGINE_VULKANWINDOWCONTEXT_H