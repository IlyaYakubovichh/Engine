#include "VulkanRendererAPI.h"
#include "VulkanUtils.h"
#include "Resources/VulkanImage.h"
#include "Vulkan/VulkanSystem.h"
#include "Window/WindowSystem.h"
#include "Log/LogSystem.h"

namespace Engine {

    static constexpr std::string_view LOG_CATEGORY = "VulkanRendererAPI";

    void VulkanRendererAPI::Initialize()
    {
        const auto* vulkan = VulkanSystem::GetInstance();

        mDevice             = vulkan->GetVkDevice();
        mGraphicsQueue      = vulkan->GetVkGraphicsQueue();
        mPresentQueue       = vulkan->GetVkPresentQueue();
        mGraphicsQueueIndex = vulkan->GetVkGraphicsQueueIndex();

        ENGINE_ASSERT_MSG(mDevice != VK_NULL_HANDLE, "VulkanRendererAPI: device is null");
        ENGINE_ASSERT_MSG(mGraphicsQueue != VK_NULL_HANDLE, "VulkanRendererAPI: graphics queue is null");

        ENGINE_LOG_INFO(LOG_CATEGORY, "Initialized");
    }

    void VulkanRendererAPI::Shutdown()
    {
        vkDeviceWaitIdle(mDevice);

        for (auto& [id, state] : mWindows) {
            for (auto& frame : state->frames)
                frame.Destroy(mDevice);

            if (state->swapchain)
                state->swapchain->Shutdown(mDevice);

            VulkanSystem::GetInstance()->DestroySurface(id);
        }

        mWindows.clear();
        ENGINE_LOG_INFO(LOG_CATEGORY, "Shutdown");
    }

    void VulkanRendererAPI::AddWindow(uint32_t windowId)
    {
        ENGINE_ASSERT_MSG(!mWindows.contains(windowId), "VulkanRendererAPI: window already registered");

        const auto* vulkan = VulkanSystem::GetInstance();

        VkSurfaceKHR surface = vulkan->GetSurface(windowId);
        ENGINE_ASSERT_MSG(surface != VK_NULL_HANDLE, "VulkanRendererAPI: surface is null for window");

        auto window = WindowSystem::GetInstance()->GetWindowById(windowId);
        ENGINE_ASSERT_MSG(window, "VulkanRendererAPI: window not found in WindowSystem");

        auto [w, h] = window->GetExtent();

        auto state = std::make_unique<WindowRenderState>();
        state->surface = surface;
        state->swapchain = std::make_shared<VulkanSwapchain>(VulkanSwapchainSettings{
            .physicalDevice     = vulkan->GetVkPhysicalDevice(),
            .device             = mDevice,
            .surface            = surface,
            .width              = static_cast<uint32_t>(w),
            .height             = static_cast<uint32_t>(h),
            });

        for (auto& frame : state->frames)
            frame.Initialize(mDevice, mGraphicsQueueIndex);

        mWindows.emplace(windowId, std::move(state));
        ENGINE_LOG_INFO(LOG_CATEGORY, "Window {} added ({} frames in flight)", windowId, gMaxFramesInFlight);
    }

    void VulkanRendererAPI::RemoveWindow(uint32_t windowId)
    {
        const auto it = mWindows.find(windowId);
        ENGINE_ASSERT_MSG(it != mWindows.end(), "VulkanRendererAPI: window not found");

        vkDeviceWaitIdle(mDevice);

        for (auto& frame : it->second->frames)
            frame.Destroy(mDevice);

        if (it->second->swapchain)
            it->second->swapchain->Shutdown(mDevice);

        mWindows.erase(it);

        VulkanSystem::GetInstance()->DestroySurface(windowId);

        ENGINE_LOG_INFO(LOG_CATEGORY, "Window {} removed", windowId);
    }

    void VulkanRendererAPI::BeginFrame() 
    {
        // TODO
    }

    void VulkanRendererAPI::EndFrame() 
    {
        // TODO
    }

    void VulkanRendererAPI::BeginWindow(uint32_t windowId)
    {
        ENGINE_ASSERT_MSG(!mActiveContext, "VulkanRendererAPI: BeginWindow called without closing previous window");

        const auto it = mWindows.find(windowId);
        ENGINE_ASSERT_MSG(it != mWindows.end(), "VulkanRendererAPI: window not registered");

        mActiveContext = it->second.get();

        WaitForFrame(*mActiveContext);
        AcquireImage(*mActiveContext);
        mActiveContext->CurrentFrame().BeginCommandBuffer();
    }

    void VulkanRendererAPI::EndWindow()
    {
        ENGINE_ASSERT_MSG(mActiveContext, "VulkanRendererAPI: EndWindow called without BeginWindow");
        ENGINE_ASSERT_MSG(mActiveContext->activeRenderTarget, "VulkanRendererAPI: EndWindow called with no render target set");

        WindowRenderState& ctx      = *mActiveContext;
        VkCommandBuffer    cmd      = ctx.CurrentFrame().GetCommandBuffer();
        VkImage            swapImg  = ctx.swapchain->GetImages()[ctx.imageIndex];

        TransitionRTToTransferSrc(cmd, *ctx.activeRenderTarget);
        TransitionSwapchainToTransferDst(cmd, swapImg);
        BlitRTToSwapchain(cmd, *ctx.activeRenderTarget, swapImg, ctx.swapchain->GetExtent());
        TransitionSwapchainToPresent(cmd, swapImg);

        ctx.CurrentFrame().EndCommandBuffer();

        SubmitWindow(ctx);
        PresentWindow(ctx);

        ctx.currentFrameIndex   = (ctx.currentFrameIndex + 1) % gMaxFramesInFlight;
        ctx.activeRenderTarget  = nullptr;
        mActiveContext          = nullptr;
    }

    void VulkanRendererAPI::SetRenderTarget(Ref<Image> target)
    {
        ENGINE_ASSERT_MSG(mActiveContext, "VulkanRendererAPI: SetRenderTarget called outside BeginWindow/EndWindow");
        ENGINE_ASSERT_MSG(target, "VulkanRendererAPI: render target is null");
        ENGINE_ASSERT_MSG(!mActiveContext->activeRenderTarget, "VulkanRendererAPI: render target already set for this window scope");

        mActiveContext->activeRenderTarget = std::static_pointer_cast<VulkanImage>(target);
    }

    void VulkanRendererAPI::Clear(glm::vec4 color)
    {
        ENGINE_ASSERT_MSG(mActiveContext, "VulkanRendererAPI: Clear called outside BeginWindow/EndWindow");
        ENGINE_ASSERT_MSG(mActiveContext->activeRenderTarget, "VulkanRendererAPI: Clear called with no render target");

        VkCommandBuffer cmd = mActiveContext->CurrentFrame().GetCommandBuffer();
        auto* sync = VulkanSystem::GetInstance()->GetSyncSubsystem().get();
        auto& img  = *mActiveContext->activeRenderTarget;

        sync->Transition(cmd, img, {
            .stageMask  = VK_PIPELINE_STAGE_2_CLEAR_BIT,
            .accessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .layout     = VK_IMAGE_LAYOUT_GENERAL,
            });

        const VkClearColorValue clearValue{ { color.r, color.g, color.b, color.a } };
        const VkImageSubresourceRange range{
            .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
            .baseMipLevel   = 0,
            .levelCount     = 1,
            .baseArrayLayer = 0,
            .layerCount     = 1,
        };
        vkCmdClearColorImage(cmd, img.GetVkImage(), VK_IMAGE_LAYOUT_GENERAL, &clearValue, 1, &range);
    }

    void VulkanRendererAPI::BeginRenderPass()
    {
        ENGINE_ASSERT_MSG(mActiveContext, "VulkanRendererAPI: BeginRenderPass called outside BeginWindow/EndWindow");
        ENGINE_ASSERT_MSG(mActiveContext->activeRenderTarget, "VulkanRendererAPI: BeginRenderPass called with no render target");

        VkCommandBuffer cmd = mActiveContext->CurrentFrame().GetCommandBuffer();
        auto* sync  = VulkanSystem::GetInstance()->GetSyncSubsystem().get();
        auto& img   = *mActiveContext->activeRenderTarget;

        sync->Transition(cmd, img, {
            .stageMask  = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT,
            .accessMask = VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT,
            .layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            });

        const VkRenderingAttachmentInfo colorAttachment{
            .sType          = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO,
            .imageView      = img.GetVkImageView(),
            .imageLayout    = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .loadOp         = VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp        = VK_ATTACHMENT_STORE_OP_STORE,
        };
        const VkRenderingInfo renderingInfo{
            .sType                  = VK_STRUCTURE_TYPE_RENDERING_INFO,
            .renderArea             = { {0, 0}, { img.GetWidth(), img.GetHeight() } },
            .layerCount             = 1,
            .colorAttachmentCount   = 1,
            .pColorAttachments      = &colorAttachment,
        };
        vkCmdBeginRendering(cmd, &renderingInfo);
    }

    void VulkanRendererAPI::EndRenderPass()
    {
        ENGINE_ASSERT_MSG(mActiveContext, "VulkanRendererAPI: EndRenderPass called outside BeginWindow/EndWindow");
        vkCmdEndRendering(mActiveContext->CurrentFrame().GetCommandBuffer());
    }

    // ─── Private helpers ──────────────────────────────────────────────────────────

    void VulkanRendererAPI::WaitForFrame(WindowRenderState& ctx)
    {
        VkFence fence = ctx.CurrentFence();
        vkWaitForFences(mDevice, 1, &fence, VK_TRUE, UINT64_MAX);
        vkResetFences(mDevice, 1, &fence);
    }

    void VulkanRendererAPI::AcquireImage(WindowRenderState& ctx)
    {
        ctx.imageIndex = ctx.swapchain->AcquireNextImage(mDevice, ctx.ImageAvailableSemaphore());
    }

    void VulkanRendererAPI::SubmitWindow(WindowRenderState& ctx)
    {
        const VkCommandBufferSubmitInfo cmdInfo{
            .sType          = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO,
            .commandBuffer  = ctx.CurrentFrame().GetCommandBuffer(),
        };
        const VkSemaphoreSubmitInfo waitInfo{
            .sType      = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore  = ctx.ImageAvailableSemaphore(),
            .stageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
        };
        const VkSemaphoreSubmitInfo signalInfo{
            .sType      = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO,
            .semaphore  = ctx.RenderFinishedSemaphore(),
            .stageMask  = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT,
        };
        const VkSubmitInfo2 submitInfo{
            .sType                      = VK_STRUCTURE_TYPE_SUBMIT_INFO_2,
            .waitSemaphoreInfoCount     = 1,
            .pWaitSemaphoreInfos        = &waitInfo,
            .commandBufferInfoCount     = 1,
            .pCommandBufferInfos        = &cmdInfo,
            .signalSemaphoreInfoCount   = 1,
            .pSignalSemaphoreInfos      = &signalInfo,
        };
        VK_CHECK(vkQueueSubmit2(mGraphicsQueue, 1, &submitInfo, ctx.CurrentFence()));
    }

    void VulkanRendererAPI::PresentWindow(WindowRenderState& ctx)
    {
        VkSemaphore    semaphore = ctx.RenderFinishedSemaphore();
        VkSwapchainKHR swapchain = ctx.swapchain->GetSwapchain();

        const VkPresentInfoKHR presentInfo{
            .sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores    = &semaphore,
            .swapchainCount     = 1,
            .pSwapchains        = &swapchain,
            .pImageIndices      = &ctx.imageIndex,
        };
        VK_CHECK(vkQueuePresentKHR(mPresentQueue, &presentInfo));
    }

    void VulkanRendererAPI::TransitionRTToTransferSrc(VkCommandBuffer cmd, VulkanImage& rt)
    {
        VulkanSystem::GetInstance()->GetSyncSubsystem()->Transition(cmd, rt, {
            .stageMask  = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .accessMask = VK_ACCESS_2_TRANSFER_READ_BIT,
            .layout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            });
    }

    void VulkanRendererAPI::TransitionSwapchainToTransferDst(VkCommandBuffer cmd, VkImage swapImg)
    {
        VulkanSystem::GetInstance()->GetSyncSubsystem()->InsertBarrier(cmd, {
            .sType              = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask       = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT,
            .srcAccessMask      = VK_ACCESS_2_NONE,
            .dstStageMask       = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .dstAccessMask      = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .oldLayout          = VK_IMAGE_LAYOUT_UNDEFINED,
            .newLayout          = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .image              = swapImg,
            .subresourceRange   = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
            });
    }

    void VulkanRendererAPI::BlitRTToSwapchain(VkCommandBuffer cmd, VulkanImage& rt, VkImage swapImg, VkExtent2D swapExtent)
    {
        const VkImageBlit2 blitRegion{
            .sType              = VK_STRUCTURE_TYPE_IMAGE_BLIT_2,
            .srcSubresource     = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .srcOffsets         = { VkOffset3D{0,0,0}, { (int32_t)rt.GetWidth(), (int32_t)rt.GetHeight(), 1 } },
            .dstSubresource     = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
            .dstOffsets         = { VkOffset3D{0,0,0}, { (int32_t)swapExtent.width, (int32_t)swapExtent.height, 1 } },
        };
        const VkBlitImageInfo2 blitInfo{
            .sType              = VK_STRUCTURE_TYPE_BLIT_IMAGE_INFO_2,
            .srcImage           = rt.GetVkImage(),
            .srcImageLayout     = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            .dstImage           = swapImg,
            .dstImageLayout     = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .regionCount        = 1,
            .pRegions           = &blitRegion,
            .filter             = VK_FILTER_LINEAR,
        };
        vkCmdBlitImage2(cmd, &blitInfo);
    }

    void VulkanRendererAPI::TransitionSwapchainToPresent(VkCommandBuffer cmd, VkImage swapImg)
    {
        VulkanSystem::GetInstance()->GetSyncSubsystem()->InsertBarrier(cmd, {
            .sType              = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask       = VK_PIPELINE_STAGE_2_TRANSFER_BIT,
            .srcAccessMask      = VK_ACCESS_2_TRANSFER_WRITE_BIT,
            .dstStageMask       = VK_PIPELINE_STAGE_2_BOTTOM_OF_PIPE_BIT,
            .dstAccessMask      = VK_ACCESS_2_NONE,
            .oldLayout          = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            .newLayout          = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
            .image              = swapImg,
            .subresourceRange   = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 },
            });
    }

} // namespace Engine