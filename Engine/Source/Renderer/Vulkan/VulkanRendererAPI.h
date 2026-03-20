#pragma once
#include "Renderer/RendererAPI.h"
#include "VulkanRendererData.h"
#include "Macros.h"
#include <glm/glm.hpp>

namespace Engine {

    /**
     * @brief Vulkan implementation of RendererAPI.
     *
     * Drives frame-in-flight logic, swapchain acquisition, render target
     * blitting, and dynamic rendering (VK_KHR_dynamic_rendering).
     *
     * TODO (multi-window): store one VulkanRendererData per window ID
     * and accept a windowId in BeginFrame / Present to drive each swapchain.
     */
    class ENGINE_API VulkanRendererAPI final : public RendererAPI {
    public:
        void Initialize() override;
        void Shutdown()   override;

        void BeginFrame()      override;
        void EndFrame()        override;
        void BeginRenderPass() override;
        void EndRenderPass()   override;
        void Present()         override;

        void SetRenderTarget(Ref<Image> target) override;
        void Clear(glm::vec4 clearColor)        override;

    private:
        // ── BeginFrame helpers ────────────────────────────────────────────────────
        void WaitForFrame();
        void AcquireSwapchainImage();

        // ── EndFrame helpers ──────────────────────────────────────────────────────
        void TransitionRenderTargetToTransferSrc(VkCommandBuffer cmd);
        void TransitionSwapchainToTransferDst(VkCommandBuffer cmd, VkImage swapchainImage);
        void BlitRenderTargetToSwapchain(VkCommandBuffer cmd, VkImage swapchainImage);
        void TransitionSwapchainToPresent(VkCommandBuffer cmd, VkImage swapchainImage);
        void SubmitFrame();

        VulkanRendererData mData;
    };

} // namespace Engine