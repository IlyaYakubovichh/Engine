#pragma once
#include "VulkanFrame.h"
#include "Resources/VulkanImage.h"
#include "Vulkan/VulkanSwapchain.h"
#include <array>
#include <vulkan/vulkan.h>

namespace Engine {

    inline constexpr uint32_t gMaxFramesInFlight = 3;

    struct WindowRenderState {
        VkSurfaceKHR                                surface{ VK_NULL_HANDLE }; 
        Ref<VulkanSwapchain>                        swapchain;                 
        std::array<VulkanFrame, gMaxFramesInFlight> frames;
        uint32_t                                    currentFrameIndex{ 0 };
        uint32_t                                    imageIndex{ 0 };
        Ref<VulkanImage>                            activeRenderTarget;

        [[nodiscard]] VulkanFrame&       CurrentFrame()         { return frames[currentFrameIndex];                     }
        [[nodiscard]] const VulkanFrame& CurrentFrame()  const  { return frames[currentFrameIndex];                     }
        [[nodiscard]] VkFence            CurrentFence()  const  { return frames[currentFrameIndex].GetInFlightFence();  }
        [[nodiscard]] VkSemaphore        ImageAvailableSemaphore() const {
            return frames[currentFrameIndex].GetImageAvailableSemaphore();
        }
        [[nodiscard]] VkSemaphore RenderFinishedSemaphore() const {
            return swapchain->GetRenderFinishedSemaphore(imageIndex);
        }
    };

} // namespace Engine