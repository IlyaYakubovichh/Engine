#pragma once

#include "Macros.h"
#include "Utils.h"
#include "VulkanSwapchain.h"
#include <vulkan/vulkan.h>

namespace Engine {

    /**
     * @brief Per-window Vulkan state: surface + swapchain.
     *
     * Owned by VulkanSystem and keyed by the WindowSystem window ID.
     */
    struct ENGINE_API VulkanWindowContext {
        VkSurfaceKHR         surface{ VK_NULL_HANDLE };
        Ref<VulkanSwapchain> swapchain;
    };

} // namespace Engine