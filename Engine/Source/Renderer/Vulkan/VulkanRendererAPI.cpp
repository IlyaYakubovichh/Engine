#include "VulkanRendererAPI.h"
#include "Renderer/Vulkan/VulkanImage.h"
#include "VulkanSystem/VulkanSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtils.h"

namespace Engine {

    static constexpr std::string_view LOG_CATEGORY = "VulkanRendererAPI";

    // ─── Lifecycle ────────────────────────────────────────────────────────────────

    void VulkanRendererAPI::Initialize(const RendererSettings& settings)
    {
        const auto* vulkan = VulkanSystem::GetInstance();

        mData.device            = vulkan->GetVkDevice();
        mData.graphicsQueue     = vulkan->GetVkGraphicsQueue();
        mData.presentQueue      = vulkan->GetVkPresentationQueue();
        mData.swapchain         = vulkan->GetWindowContext(settings.windowId).swapchain;

        ENGINE_ASSERT_MSG(mData.device != VK_NULL_HANDLE,           "VulkanRendererAPI: device is null");
        ENGINE_ASSERT_MSG(mData.graphicsQueue != VK_NULL_HANDLE,    "VulkanRendererAPI: graphics queue is null");
        ENGINE_ASSERT_MSG(mData.swapchain != nullptr,               "VulkanRendererAPI: swapchain is null");

        const uint32_t queueIndex = vulkan->GetVkGraphicsQueueIndex();
        for (auto& frame : mData.frames) {
            frame.Initialize(mData.device, queueIndex);
        }

        ENGINE_LOG_INFO(LOG_CATEGORY, "Initialized (window {}, {} frames in flight)", settings.windowId, gMaxFramesInFlight);
    }

    void VulkanRendererAPI::Shutdown()
    {
        vkDeviceWaitIdle(mData.device);

        for (auto& frame : mData.frames) {
            frame.Destroy(mData.device);
        }

        ENGINE_LOG_INFO(LOG_CATEGORY, "Shutdown");
    }

    // ─── Frame ────────────────────────────────────────────────────────────────────

    void VulkanRendererAPI::BeginFrame()
    {
        WaitForFrame();
        AcquireSwapchainImage();
        mData.CurrentFrame().BeginCommandBuffer();
    }

    void VulkanRendererAPI::EndFrame()
    {
        ENGINE_ASSERT_MSG(mData.activeRenderTarget != nullptr, "VulkanRendererAPI: EndFrame called without a valid render target");

        VkCommandBuffer cmd = mData.CurrentFrame().GetCommandBuffer();
        VkImage         swapchainImg = mData.swapchain->GetImages()[mData.imageIndex];

        TransitionRenderTargetToTransferSrc(cmd);
        TransitionSwapchainToTransferDst(cmd, swapchainImg);
        BlitRenderTargetToSwapchain(cmd, swapchainImg);
        TransitionSwapchainToPresent(cmd, swapchainImg);

        mData.CurrentFrame().EndCommandBuffer();
        SubmitFrame();

        mData.activeRenderTarget = nullptr;
    }

    void VulkanRendererAPI::BeginRenderPass()
    {
        ENGINE_ASSERT_MSG(mData.activeRenderTarget != nullptr, "VulkanRendererAPI: BeginRenderPass called without a render target");

        VkCommandBuffer cmd = mData.CurrentFrame().GetCommandBuffer();
        auto* sync = VulkanSystem::GetInstance()->GetSyncSubsystem().get();
        auto& img = *mData.activeRenderTarget;

        sync->Transition(cmd, img, {
            .stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .accessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            });

        const VkRenderingAttachmentInfo colorAttachment{
            .sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView = img.GetVkImageView(),
            .imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        };
        const VkRenderingInfo renderingInfo{
            .sType = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea = { {0, 0}, { img.GetWidth(), img.GetHeight() } },
            .layerCount = 1,
            .colorAttachmentCount = 1,
            .pColorAttachments = &colorAttachment,
        };

        vkCmdBeginRendering(cmd, &renderingInfo);
    }

    void VulkanRendererAPI::EndRenderPass()
    {
        vkCmdEndRendering(mData.CurrentFrame().GetCommandBuffer());
    }

    void VulkanRendererAPI::Present()
    {
        VkSemaphore    semaphore = mData.CurrentRenderFinishedSemaphore();
        VkSwapchainKHR swapchain = mData.swapchain->GetSwapchain();

        const VkPresentInfoKHR presentInfo{
            .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &semaphore,
            .swapchainCount = 1,
            .pSwapchains = &swapchain,
            .pImageIndices = &mData.imageIndex,
        };

        VK_CHECK(vkQueuePresentKHR(mData.presentQueue, &presentInfo));

        // TODO (multi-window): advance frame index per-window, not globally.
        mData.currentFrameIndex = (mData.currentFrameIndex + 1) % gMaxFramesInFlight;
    }

    void VulkanRendererAPI::SetRenderTarget(Ref<Image> target)
    {
        ENGINE_ASSERT_MSG(target != nullptr, "VulkanRendererAPI: render target must not be null");
        mData.activeRenderTarget = std::static_pointer_cast<VulkanImage>(target);
    }

    void VulkanRendererAPI::Clear(glm::vec4 color)
    {
        ENGINE_ASSERT_MSG(mData.activeRenderTarget != nullptr, "VulkanRendererAPI: Clear called without an active render target");

        VkCommandBuffer cmd = mData.CurrentFrame().GetCommandBuffer();
        auto* sync = VulkanSystem::GetInstance()->GetSyncSubsystem().get();
        auto& img = *mData.activeRenderTarget;

        sync->Transition(cmd, img, {
            .stageMask = VK_PIPELINE_STAGE_2_CLEAR_BIT,
            .accessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .layout = VK_IMAGE_LAYOUT_GENERAL,
            });

        const VkClearColorValue clearValue{ { color.r, color.g, color.b, color.a } };
        const VkImageSubresourceRange range{
            .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel = 0,
            .levelCount = 1,
            .baseArrayLayer = 0,
            .layerCount = 1,
        };

        vkCmdClearColorImage(cmd, img.GetVkImage(),
            VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &range);
    }

    // ─── BeginFrame helpers ───────────────────────────────────────────────────────

    void VulkanRendererAPI::WaitForFrame()
    {
        VkFence fence = mData.CurrentFrame().GetInFlightFence();
        vkWaitForFences(mData.device, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(mData.device, 1, &fence);
    }

    void VulkanRendererAPI::AcquireSwapchainImage()
    {
        mData.imageIndex = mData.swapchain->AcquireNextImage(
            mData.device,
            mData.CurrentFrame().GetImageAvailableSemaphore());
    }

    // ─── EndFrame helpers ─────────────────────────────────────────────────────────

    void VulkanRendererAPI::TransitionRenderTargetToTransferSrc(VkCommandBuffer cmd)
    {
        auto* sync = VulkanSystem::GetInstance()->GetSyncSubsystem().get();
        sync->Transition(cmd, *mData.activeRenderTarget, {
            .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .accessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            });
    }

    void VulkanRendererAPI::TransitionSwapchainToTransferDst(VkCommandBuffer cmd, VkImage swapchainImage)
    {
        auto* sync = VulkanSystem::GetInstance()->GetSyncSubsystem().get();
        sync->InsertBarrier(cmd, {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask = VK_ACCESS_2_NONE,
            .dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .image = swapchainImage,
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
            });
    }

    void VulkanRendererAPI::BlitRenderTargetToSwapchain(VkCommandBuffer cmd, VkImage swapchainImage)
    {
        const VkExtent2D swapExtent = mData.swapchain->GetExtent();
        const auto& rt = *mData.activeRenderTarget;

        const VkImageBlit2 blitRegion{
            .sType = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .srcSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .srcOffsets = { VkOffset3D{0, 0, 0},
                                VkOffset3D{ (int32_t)rt.GetWidth(), (int32_t)rt.GetHeight(), 1 } },
            .dstSubresource = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .dstOffsets = { VkOffset3D{0, 0, 0},
                                VkOffset3D{ (int32_t)swapExtent.width, (int32_t)swapExtent.height, 1 } },
        };

        const VkBlitImageInfo2 blitInfo{
            .sType = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .srcImage = rt.GetVkImage(),
            .srcImageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage = swapchainImage,
            .dstImageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount = 1,
            .pRegions = &blitRegion,
            .filter = VK_FILTER_LINEAR,
        };

        vkCmdBlitImage2(cmd, &blitInfo);
    }

    void VulkanRendererAPI::TransitionSwapchainToPresent(VkCommandBuffer cmd, VkImage swapchainImage)
    {
        auto* sync = VulkanSystem::GetInstance()->GetSyncSubsystem().get();
        sync->InsertBarrier(cmd, {
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask = VK_ACCESS_2_NONE,
            .oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image = swapchainImage,
            .subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
            });
    }

    void VulkanRendererAPI::SubmitFrame()
    {
        const VkCommandBufferSubmitInfo cmdInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer = mData.CurrentFrame().GetCommandBuffer(),
        };
        const VkSemaphoreSubmitInfo waitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = mData.CurrentFrame().GetImageAvailableSemaphore(),
            .stageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        };
        const VkSemaphoreSubmitInfo signalInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore = mData.CurrentRenderFinishedSemaphore(),
            .stageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        };
        const VkSubmitInfo2 submitInfo{
            .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount = 1,
            .pWaitSemaphoreInfos = &waitInfo,
            .commandBufferInfoCount = 1,
            .pCommandBufferInfos = &cmdInfo,
            .signalSemaphoreInfoCount = 1,
            .pSignalSemaphoreInfos = &signalInfo,
        };

        VK_CHECK(vkQueueSubmit2(mData.graphicsQueue, 1, &submitInfo,
            mData.CurrentInFlightFence()));
    }

} // namespace Engine