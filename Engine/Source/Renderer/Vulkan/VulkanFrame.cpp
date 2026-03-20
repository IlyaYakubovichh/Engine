#include "VulkanFrame.h"
#include "Systems/LogSystem/LogSystem.h"
#include "VulkanUtils.h"

namespace Engine {

    void VulkanFrame::Initialize(VkDevice device, const uint32_t graphicsQueueFamilyIndex)
    {
        ENGINE_ASSERT_MSG(device != VK_NULL_HANDLE, "VulkanFrame: device is null");
        ENGINE_ASSERT_MSG(!mInitialized, "VulkanFrame: already initialized");

        // Command pool
        const VkCommandPoolCreateInfo poolInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = graphicsQueueFamilyIndex,
        };
        VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &mCommandPool));
        ENGINE_ASSERT_MSG(mCommandPool != VK_NULL_HANDLE, "VulkanFrame: failed to create command pool");

        // Primary command buffer
        const VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool = mCommandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &mCommandBuffer));
        ENGINE_ASSERT_MSG(mCommandBuffer != VK_NULL_HANDLE, "VulkanFrame: failed to allocate command buffer");

        // Image-available semaphore
        constexpr VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mImageAvailableSemaphore));
        ENGINE_ASSERT_MSG(mImageAvailableSemaphore != VK_NULL_HANDLE, "VulkanFrame: failed to create semaphore");

        // In-flight fence — pre-signalled so the first frame doesn't stall
        constexpr VkFenceCreateInfo fenceInfo{
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &mInFlightFence));
        ENGINE_ASSERT_MSG(mInFlightFence != VK_NULL_HANDLE, "VulkanFrame: failed to create fence");

        mInitialized = true;
        ENGINE_LOG_DEBUG("VulkanFrame", "Initialized");
    }

    void VulkanFrame::Destroy(VkDevice device)
    {
        if (!mInitialized) return;
        ENGINE_ASSERT_MSG(device != VK_NULL_HANDLE, "VulkanFrame: device is null");

        if (mImageAvailableSemaphore != VK_NULL_HANDLE)
            vkDestroySemaphore(device, mImageAvailableSemaphore, nullptr);

        if (mInFlightFence != VK_NULL_HANDLE)
            vkDestroyFence(device, mInFlightFence, nullptr);

        // Destroying the pool implicitly frees mCommandBuffer.
        if (mCommandPool != VK_NULL_HANDLE)
            vkDestroyCommandPool(device, mCommandPool, nullptr);

        mImageAvailableSemaphore    = VK_NULL_HANDLE;
        mInFlightFence              = VK_NULL_HANDLE;
        mCommandPool                = VK_NULL_HANDLE;
        mCommandBuffer              = VK_NULL_HANDLE;
        mInitialized                = false;

        ENGINE_LOG_DEBUG("VulkanFrame", "Destroyed");
    }

    void VulkanFrame::BeginCommandBuffer() const
    {
        ENGINE_ASSERT_MSG(mInitialized, "VulkanFrame: not initialized");

        VK_CHECK(vkResetCommandBuffer(mCommandBuffer, 0));

        constexpr VkCommandBufferBeginInfo beginInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VK_CHECK(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
    }

    void VulkanFrame::EndCommandBuffer() const
    {
        ENGINE_ASSERT_MSG(mInitialized, "VulkanFrame: not initialized");
        VK_CHECK(vkEndCommandBuffer(mCommandBuffer));
    }

} // namespace Engine