#include "VulkanRendererAPI.h"
#include "VulkanSystem/VulkanSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"

namespace Engine {

    static constexpr std::string_view LOG_CATEGORY = "VulkanRendererAPI";

    VulkanRendererAPI::VulkanRendererAPI() {
        mAPI = API::Vulkan;
    }

    void VulkanRendererAPI::Initialize() {
        const auto* vulkan = VulkanSystem::GetInstance();

        mData.device        = vulkan->GetVkDevice();
        mData.graphicsQueue = vulkan->GetVkGraphicsQueue();
        mData.presentQueue  = vulkan->GetVkPresentationQueue();
        mData.swapchain     = vulkan->GetWindowContext(gMainWindowId).swapchain;

        ENGINE_ASSERT_MESSAGE(mData.device        != VK_NULL_HANDLE, "VulkanRendererAPI: device is null");
        ENGINE_ASSERT_MESSAGE(mData.graphicsQueue != VK_NULL_HANDLE, "VulkanRendererAPI: graphics queue is null");
        ENGINE_ASSERT_MESSAGE(mData.swapchain     != nullptr,        "VulkanRendererAPI: swapchain is null");

        const uint32_t graphicsQueueIndex = vulkan->GetVkGraphicsQueueIndex();
        for (auto& frame : mData.frames)
            frame.Initialize(mData.device, graphicsQueueIndex);

        ENGINE_LOG_INFO(LOG_CATEGORY, "Initialized ({} frames in flight)", MAX_FRAMES_IN_FLIGHT);
    }

    void VulkanRendererAPI::Shutdown() {
        vkDeviceWaitIdle(mData.device);

        for (auto& frame : mData.frames)
            frame.Destroy(mData.device);

        ENGINE_LOG_INFO(LOG_CATEGORY, "Shutdown");
    }

    // Wait for fence and begin command buffer recording.
    void VulkanRendererAPI::BeginFrame() {
        const auto& frame = mData.CurrentFrame();

        // Wait for GPU finish the frame.
        VkFence inFlightFence = frame.GetInFlightFence();
        vkWaitForFences(mData.device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences  (mData.device, 1, &inFlightFence);

        // Get available image index.
        mData.imageIndex = mData.swapchain->AcquireNextImage(mData.device, frame.GetImageAvailableSemaphore());

        // Begin command buffer recording.
        frame.BeginCommandBuffer();
    }

    // Stop recording and submit work to GPU.
    void VulkanRendererAPI::EndFrame() {
        const auto& frame = mData.CurrentFrame();

        // Transition: UNDEFINED -> PRESENT_SRC_KHR
        VkImageMemoryBarrier2 barrier {
            .sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask        = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask       = VK_ACCESS_2_NONE,
            .dstStageMask        = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask       = VK_ACCESS_2_NONE,
            .oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image               = mData.swapchain->GetImages()[mData.imageIndex],
            .subresourceRange    = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        const VkDependencyInfo dep {
            .sType                   = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers    = &barrier,
        };

        vkCmdPipelineBarrier2(frame.GetCommandBuffer(), &dep);

        frame.EndCommandBuffer();

        // Command buffer submit info.
        const VkCommandBufferSubmitInfo commandBufferSubmitInfo {
            .sType         = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = frame.GetCommandBuffer(),
            .deviceMask    = 0,
        };

        // Semaphore wait info. Waiting on COLOR_OUTPUT for imageAvailableSemaphore.
        const VkSemaphoreSubmitInfo waitInfo {
            .sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frame.GetImageAvailableSemaphore(),
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
        };

        // Semaphore signal operation. Signal renderFinishedSemaphore after all graphics stages complete.
        VkSemaphore submitSemaphore = mData.swapchain->GetRenderFinishedSemaphore(mData.imageIndex);
        const VkSemaphoreSubmitInfo signalInfo {
            .sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = submitSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        };

        // Submit batches to GPU.
        const VkSubmitInfo2 submitInfo {
            .sType                    = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount   = 1,
            .pWaitSemaphoreInfos      = &waitInfo,
            .commandBufferInfoCount   = 1,
            .pCommandBufferInfos      = &commandBufferSubmitInfo,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos    = &signalInfo,
        };

        VULKAN_CHECK(vkQueueSubmit2(mData.graphicsQueue, 1, &submitInfo, frame.GetInFlightFence()));
    }

    // Open render pass(scope where render commands begin).
    void VulkanRendererAPI::BeginRenderPass() {
        auto& frame = mData.CurrentFrame();
    }

    // Close render pass(scope where render commands end).
    void VulkanRendererAPI::EndRenderPass() {
        auto& frame = mData.CurrentFrame();
    }

    // Present result to the surface.
    void VulkanRendererAPI::Present() {
        VkSemaphore submitSemaphore  = mData.swapchain->GetRenderFinishedSemaphore(mData.imageIndex);
        VkSwapchainKHR swapchain     = mData.swapchain->GetSwapchain();

        // Present.
        const VkPresentInfoKHR presentInfo {
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &submitSemaphore,
            .swapchainCount     = 1,
            .pSwapchains        = &swapchain,
            .pImageIndices      = &mData.imageIndex,
        };

        VULKAN_CHECK(vkQueuePresentKHR(mData.presentQueue, &presentInfo));

        mData.currentFrameIndex = (mData.currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    /*
     * Semaphore reuse problem: there is no way to wait for vkQueuePresentKHR, so we can't know if vkQueuePresentKHR is done using that semaphore.
     * For that we do: count of render semaphores = count of swapchain images + we index semaphores via acquire image index.
     */

    void VulkanRendererAPI::Clear(glm::vec4 clearColor) {
        auto& frame = mData.CurrentFrame();

        VkClearColorValue clearValue = {{clearColor.r, clearColor.g, clearColor.b, clearColor.a}};

        VkImage image = mData.swapchain->GetImages()[mData.imageIndex];
        VkImageSubresourceRange subresourceRange{};
        subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        subresourceRange.baseMipLevel = 0;
        subresourceRange.levelCount = 1;
        subresourceRange.baseArrayLayer = 0;
        subresourceRange.layerCount = 1;

        vkCmdClearColorImage(frame.GetCommandBuffer(), image, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &subresourceRange);
    }

} // Engine