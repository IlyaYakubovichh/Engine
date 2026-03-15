#include "VulkanRendererAPI.h"
#include "VulkanRenderer/VulkanImage.h"
#include "VulkanSystem/VulkanSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"
#include <array>

namespace Engine {

    static constexpr std::string_view LOG_CATEGORY = "VulkanRendererAPI";

    VulkanRendererAPI::VulkanRendererAPI() {
        mAPI = API::Vulkan;
    }

    // Setup renderer data, create frames
    void VulkanRendererAPI::Initialize() {
        const auto* vulkan = VulkanSystem::GetInstance();

        mData.device = vulkan->GetVkDevice();
        mData.graphicsQueue = vulkan->GetVkGraphicsQueue();
        mData.presentQueue = vulkan->GetVkPresentationQueue();
        mData.swapchain = vulkan->GetWindowContext(gMainWindowId).swapchain;

        ENGINE_ASSERT_MESSAGE(mData.device != VK_NULL_HANDLE, "VulkanRendererAPI: device is null");
        ENGINE_ASSERT_MESSAGE(mData.graphicsQueue != VK_NULL_HANDLE, "VulkanRendererAPI: graphics queue is null");
        ENGINE_ASSERT_MESSAGE(mData.swapchain != nullptr, "VulkanRendererAPI: swapchain is null");

        const uint32_t graphicsQueueIndex = vulkan->GetVkGraphicsQueueIndex();
        for (auto& frame : mData.frames)
            frame.Initialize(mData.device, graphicsQueueIndex);

        ENGINE_LOG_INFO(LOG_CATEGORY, "Initialized ({} frames in flight)", MAX_FRAMES_IN_FLIGHT);
    }

    // Wait till device idle, destroy frames
    void VulkanRendererAPI::Shutdown() {
        vkDeviceWaitIdle(mData.device);

        for (auto& frame : mData.frames)
            frame.Destroy(mData.device);

        ENGINE_LOG_INFO(LOG_CATEGORY, "Shutdown");
    }

    // Wait for fence, acquire swapchain image, begin command buffer
    void VulkanRendererAPI::BeginFrame() {
        const auto& frame = mData.CurrentFrame();

        VkFence inFlightFence = frame.GetInFlightFence();
        vkWaitForFences(mData.device, 1, &inFlightFence, VK_TRUE, UINT64_MAX);
        vkResetFences(mData.device, 1, &inFlightFence);

        mData.imageIndex = mData.swapchain->AcquireNextImage(mData.device, frame.GetImageAvailableSemaphore());

        frame.BeginCommandBuffer();
    }

    // Blit render target -> swapchain, submit
    void VulkanRendererAPI::EndFrame() {
        const auto& frame = mData.CurrentFrame();
        VkCommandBuffer cmd = frame.GetCommandBuffer();

        ENGINE_ASSERT_MESSAGE(mData.activeRenderImage != VK_NULL_HANDLE, "VulkanRendererAPI: EndFrame called without a valid render target");

        VkImage swapchainImage = mData.swapchain->GetImages()[mData.imageIndex];

        // Transition render target: COLOR_ATTACHMENT_OPTIMAL -> TRANSFER_SRC_OPTIMAL
        VkImageMemoryBarrier2 srcBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .image = mData.activeRenderImage,
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        // Transition swapchain: UNDEFINED -> TRANSFER_DST_OPTIMAL
        VkImageMemoryBarrier2 dstBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .image = swapchainImage,
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        const std::array preBlit = { srcBarrier, dstBarrier };
        const VkDependencyInfo preDep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = static_cast<uint32_t>(preBlit.size()),
            .pImageMemoryBarriers = preBlit.data(),
        };

        vkCmdPipelineBarrier2(cmd, &preDep);

        // Blit render target -> swapchain (with scaling if sizes differ)
        const VkExtent2D swapExtent = mData.swapchain->GetExtent();

        const VkImageBlit2 blitRegion{
            .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .srcOffsets = { VkOffset3D{0, 0, 0}, VkOffset3D{ (int32_t)mData.activeRenderExtent.width,  (int32_t)mData.activeRenderExtent.height,  1 } },
            .dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .dstOffsets = { VkOffset3D{0, 0, 0}, VkOffset3D{ (int32_t)swapExtent.width, (int32_t)swapExtent.height, 1 } },
        };

        const VkBlitImageInfo2 blitInfo{
            .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .srcImage = mData.activeRenderImage,
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage = swapchainImage,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = 1,
            .pRegions = &blitRegion,
            .filter = VK_FILTER_LINEAR,
        };

        vkCmdBlitImage2(cmd, &blitInfo);

        // Transition swapchain: TRANSFER_DST_OPTIMAL -> PRESENT_SRC_KHR
        VkImageMemoryBarrier2 presentBarrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask = VK_ACCESS_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = swapchainImage,
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        const VkDependencyInfo postDep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &presentBarrier,
        };

        vkCmdPipelineBarrier2(cmd, &postDep);

        frame.EndCommandBuffer();

        // Submit
        const VkCommandBufferSubmitInfo commandBufferSubmitInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = frame.GetCommandBuffer(),
            .deviceMask = 0,
        };

        const VkSemaphoreSubmitInfo waitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = frame.GetImageAvailableSemaphore(),
            .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        };

        VkSemaphore submitSemaphore = mData.swapchain->GetRenderFinishedSemaphore(mData.imageIndex);
        const VkSemaphoreSubmitInfo signalInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = submitSemaphore,
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        };

        const VkSubmitInfo2 submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &waitInfo,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &commandBufferSubmitInfo,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &signalInfo,
        };

        VULKAN_CHECK(vkQueueSubmit2(mData.graphicsQueue, 1, &submitInfo, frame.GetInFlightFence()));
    }

    // Transition render target to COLOR_ATTACHMENT_OPTIMAL, begin dynamic rendering
    void VulkanRendererAPI::BeginRenderPass(Ref<Image> renderTarget) {
        ENGINE_ASSERT_MESSAGE(renderTarget != nullptr, "VulkanRendererAPI: renderTarget must not be null");

        VkCommandBuffer cmd = mData.CurrentFrame().GetCommandBuffer();

        auto* vkImage = static_cast<VulkanImage*>(renderTarget.get());
        mData.activeRenderImage = vkImage->GetVkImage();
        mData.activeRenderExtent = { vkImage->GetWidth(), vkImage->GetHeight() };

        // Transition: UNDEFINED -> COLOR_ATTACHMENT_OPTIMAL
        VkImageMemoryBarrier2 barrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .image = mData.activeRenderImage,
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
        };

        const VkDependencyInfo dep{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier,
        };

        vkCmdPipelineBarrier2(cmd, &dep);

        // Update image sync state
        vkImage->GetSyncState() = {
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .accessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };

        const VkRenderingAttachmentInfo colorAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = vkImage->GetVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };

        const VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = { {0, 0}, mData.activeRenderExtent },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
        };

        vkCmdBeginRendering(cmd, &renderingInfo);
    }

    void VulkanRendererAPI::EndRenderPass() {
        vkCmdEndRendering(mData.CurrentFrame().GetCommandBuffer());
    }

    // Present swapchain image
    void VulkanRendererAPI::Present() {
        VkSemaphore    submitSemaphore = mData.swapchain->GetRenderFinishedSemaphore(mData.imageIndex);
        VkSwapchainKHR swapchain = mData.swapchain->GetSwapchain();

        const VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &submitSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &mData.imageIndex,
        };

        VULKAN_CHECK(vkQueuePresentKHR(mData.presentQueue, &presentInfo));

        mData.currentFrameIndex = (mData.currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    /*
     * Semaphore reuse problem: there is no way to wait for vkQueuePresentKHR, so we can't know
     * if vkQueuePresentKHR is done using that semaphore.
     * Solution: count of renderFinished semaphores == count of swapchain images,
     * indexed by acquire image index (not frame index).
     */

     // Clear the active render target
    void VulkanRendererAPI::Clear(glm::vec4 clearColor) {
        ENGINE_ASSERT_MESSAGE(mData.activeRenderImage != VK_NULL_HANDLE, "VulkanRendererAPI: Clear called without an active render target");

        VkCommandBuffer cmd = mData.CurrentFrame().GetCommandBuffer();

        const VkClearColorValue clearValue = { { clearColor.r, clearColor.g, clearColor.b, clearColor.a } };

        const VkImageSubresourceRange subresourceRange{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        vkCmdClearColorImage(cmd, mData.activeRenderImage, VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &subresourceRange);
    }

} // namespace Engine