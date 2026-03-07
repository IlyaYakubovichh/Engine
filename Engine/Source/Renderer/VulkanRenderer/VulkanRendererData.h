//
// Created by ilya on 07.03.2026.
//

#ifndef ENGINE_VULKANRENDERERDATA_H
#define ENGINE_VULKANRENDERERDATA_H

#include "VulkanFrame.h"
#include "Utility.h"
#include "VulkanSystem/VulkanSwapchain.h"
#include <array>
#include <vulkan/vulkan.h>

namespace Engine {

    static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

    struct VulkanRendererData {
        VkDevice    device        { VK_NULL_HANDLE };
        VkQueue     graphicsQueue { VK_NULL_HANDLE };
        VkQueue     presentQueue  { VK_NULL_HANDLE };

        Ref<VulkanSwapchain> swapchain { nullptr };

        std::array<VulkanFrame, MAX_FRAMES_IN_FLIGHT> frames;
        uint32_t currentFrameIndex { 0 };
        uint32_t imageIndex        { 0 };

        // Getters
        [[nodiscard]] VulkanFrame&       CurrentFrame()       { return frames[currentFrameIndex]; }
        [[nodiscard]] const VulkanFrame& CurrentFrame() const { return frames[currentFrameIndex]; }

        [[nodiscard]] VkCommandBuffer CurrentCommandBuffer()            const { return frames[currentFrameIndex].GetCommandBuffer();            }
        [[nodiscard]] VkSemaphore     CurrentRenderFinishedSemaphore()  const { return frames[currentFrameIndex].GetRenderFinishedSemaphore();  }
        [[nodiscard]] VkFence         CurrentInFlightFence()            const { return frames[currentFrameIndex].GetInFlightFence();            }
    };

} // Engine

#endif //ENGINE_VULKANRENDERERDATA_H