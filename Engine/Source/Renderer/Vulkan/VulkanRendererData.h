#pragma once
#include "Macros.h"
#include "Utils.h"
#include "VulkanFrame.h"
#include "Renderer/Vulkan/VulkanImage.h"
#include "Systems/VulkanSystem/VulkanSwapchain.h"
#include <vulkan/vulkan.h>
#include <array>

namespace Engine {

    // Maximum number of frames processed concurrently.
    inline constexpr uint32_t gMaxFramesInFlight = 3;

    /**
     * @brief All per-renderer Vulkan state.
     *
     * Owned by VulkanRendererAPI. Accessors hide frame-index arithmetic
     * so the API methods stay readable.
     */
    struct VulkanRendererData {
        VkDevice    device{ VK_NULL_HANDLE };
        VkQueue     graphicsQueue{ VK_NULL_HANDLE };
        VkQueue     presentQueue{ VK_NULL_HANDLE };

        Ref<VulkanSwapchain> swapchain;

        std::array<VulkanFrame, gMaxFramesInFlight> frames;
        uint32_t currentFrameIndex{ 0 };
        uint32_t imageIndex{ 0 };

        // The image that BeginRenderPass / Clear / EndFrame will operate on.
        Ref<VulkanImage> activeRenderTarget;

        // ── Frame helpers ─────────────────────────────────────────────────────────

        [[nodiscard]] VulkanFrame& CurrentFrame()                           { return frames[currentFrameIndex];                         }
        [[nodiscard]] const VulkanFrame& CurrentFrame()             const   { return frames[currentFrameIndex];                         }
        [[nodiscard]] VkFence CurrentInFlightFence()                const   { return frames[currentFrameIndex].GetInFlightFence();      }
        [[nodiscard]] VkSemaphore CurrentRenderFinishedSemaphore()  const   { return swapchain->GetRenderFinishedSemaphore(imageIndex); }
    };

} // namespace Engine