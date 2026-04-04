#pragma once
#include "Macros.h"
#include "Utils.h"
#include <vulkan/vulkan.h>

namespace Engine {

    /**
     * @brief Per-frame Vulkan resources: command buffer, fence and imageAvailable semaphore.
     *
     * One instance per frame-in-flight. renderFinished semaphore lives in
     * VulkanSwapchain, indexed by swapchain image index.
     */
    class ENGINE_API VulkanFrame final : public NonCopyable {
    public:
        VulkanFrame() = default;
        ~VulkanFrame() override = default;

        void Initialize(VkDevice device, uint32_t queueFamilyIndex);
        void Destroy(VkDevice device);

        void BeginCommandBuffer() const;
        void EndCommandBuffer()   const;

        [[nodiscard]] VkCommandBuffer GetCommandBuffer()           const { return mCommandBuffer; }
        [[nodiscard]] VkSemaphore     GetImageAvailableSemaphore() const { return mImageAvailableSemaphore; }
        [[nodiscard]] VkFence         GetInFlightFence()           const { return mInFlightFence; }

    private:
        VkCommandPool   mCommandPool{ VK_NULL_HANDLE };
        VkCommandBuffer mCommandBuffer{ VK_NULL_HANDLE };
        VkSemaphore     mImageAvailableSemaphore{ VK_NULL_HANDLE };
        VkFence         mInFlightFence{ VK_NULL_HANDLE };
        bool            mInitialized{ false };
    };

} // namespace Engine