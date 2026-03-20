#pragma once

#include "Macros.h"
#include "Utils.h"
#include <vulkan/vulkan.h>

namespace Engine {

    /**
     * @brief Per-frame Vulkan resources: command buffer, sync primitives.
     *
     * One instance per frame-in-flight. Initialize() / Destroy() are called
     * explicitly by VulkanRendererAPI.
     */
    class ENGINE_API VulkanFrame final : public NonCopyable {
    public:
        VulkanFrame() = default;
        ~VulkanFrame() override = default;

        void Initialize(VkDevice device, uint32_t graphicsQueueFamilyIndex);
        void Destroy(VkDevice device);

        // Resets and begins the command buffer for this frame.
        void BeginCommandBuffer() const;

        // Ends command buffer recording.
        void EndCommandBuffer() const;

        [[nodiscard]] VkCommandBuffer GetCommandBuffer()           const { return mCommandBuffer;           }
        [[nodiscard]] VkSemaphore     GetImageAvailableSemaphore() const { return mImageAvailableSemaphore; }
        [[nodiscard]] VkFence         GetInFlightFence()           const { return mInFlightFence;           }

    private:
        VkCommandPool   mCommandPool                { VK_NULL_HANDLE };
        VkCommandBuffer mCommandBuffer              { VK_NULL_HANDLE };
        VkSemaphore     mImageAvailableSemaphore    { VK_NULL_HANDLE };
        VkFence         mInFlightFence              { VK_NULL_HANDLE };
        bool            mInitialized                { false          };
    };

} // namespace Engine