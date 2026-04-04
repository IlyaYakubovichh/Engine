#pragma once
#include "Macros.h"
#include "Utils.h"
#include "Core/RendererAPI.h"
#include "Core/WindowRenderState.h"
#include "Resources/VulkanImage.h"
#include <vulkan/vulkan.h>
#include <unordered_map>

namespace Engine {

    class ENGINE_API VulkanRendererAPI final : public RendererAPI {
    public:
        void Initialize()                    override;
        void Shutdown()                      override;
        void AddWindow(uint32_t windowId)    override;
        void RemoveWindow(uint32_t windowId) override;
        void BeginWindow(uint32_t windowId)  override;
        void EndWindow()                     override;
        void BeginFrame()                    override;
        void EndFrame()                      override;
        void BeginRenderPass()               override;
        void EndRenderPass()                 override;
        void SetRenderTarget(Ref<Image> rt)  override;
        void Clear(glm::vec4 color)          override;

    private:
        void WaitForFrame(WindowRenderState& ctx);
        void AcquireImage(WindowRenderState& ctx);
        void SubmitWindow(WindowRenderState& ctx);
        void PresentWindow(WindowRenderState& ctx);
        void TransitionRTToTransferSrc(VkCommandBuffer cmd, VulkanImage& rt);
        void TransitionSwapchainToTransferDst(VkCommandBuffer cmd, VkImage swapImg);
        void BlitRTToSwapchain(VkCommandBuffer cmd, VulkanImage& rt, VkImage swapImg, VkExtent2D swapExtent);
        void TransitionSwapchainToPresent(VkCommandBuffer cmd, VkImage swapImg);

    private:
        VkDevice mDevice                { VK_NULL_HANDLE };
        VkQueue  mGraphicsQueue         { VK_NULL_HANDLE };
        VkQueue  mPresentQueue          { VK_NULL_HANDLE };
        uint32_t mGraphicsQueueIndex    { 0 };

        std::unordered_map<uint32_t, Scope<WindowRenderState>> mWindows;
        WindowRenderState* mActiveContext{ nullptr };
    };

} // namespace Engine