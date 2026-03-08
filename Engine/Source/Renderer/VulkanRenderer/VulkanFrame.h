//
// Created by ilya on 07.03.2026.
//

#ifndef ENGINE_VULKANFRAME_H
#define ENGINE_VULKANFRAME_H

#include "Macro.h"
#include "Utility.h"
#include <vulkan/vulkan.h>

namespace Engine {

    class ENGINE_API VulkanFrame final : public NonCopyable {
    public:
        VulkanFrame() = default;
        ~VulkanFrame() override = default;

        void Initialize(VkDevice device, uint32_t graphicsQueueFamilyIndex);
        void Destroy(VkDevice device);

        void BeginCommandBuffer() const;
        void EndCommandBuffer() const;

        // Getters
        [[nodiscard]] VkCommandBuffer GetCommandBuffer()            const { return mCommandBuffer;              }
        [[nodiscard]] VkSemaphore     GetImageAvailableSemaphore()  const { return mImageAvailableSemaphore;    }
        [[nodiscard]] VkFence         GetInFlightFence()            const { return mInFlightFence;              }

    private:
        VkCommandPool   mCommandPool             { VK_NULL_HANDLE };
        VkCommandBuffer mCommandBuffer           { VK_NULL_HANDLE };
        VkSemaphore     mImageAvailableSemaphore { VK_NULL_HANDLE };
        VkFence         mInFlightFence           { VK_NULL_HANDLE };
        bool            mInitialized             { false };
    };

} // Engine

#endif // ENGINE_VULKANFRAME_H