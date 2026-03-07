//
// Created by ilya on 07.03.2026.
//

#include "VulkanFrame.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"

namespace Engine {

    void VulkanFrame::Initialize(VkDevice device, const uint32_t graphicsQueueFamilyIndex) {
        ENGINE_ASSERT_MESSAGE(device != VK_NULL_HANDLE, "VulkanFrame::Init — device is null");
        ENGINE_ASSERT_MESSAGE(!mInitialized, "VulkanFrame::Init — already initialized");

        // Command pool
        const VkCommandPoolCreateInfo poolInfo {
            .sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
            .flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
            .queueFamilyIndex = graphicsQueueFamilyIndex,
        };
        VULKAN_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &mCommandPool));
        ENGINE_ASSERT_MESSAGE(mCommandPool != VK_NULL_HANDLE, "VulkanFrame::Init — failed to create command pool");

        // Allocate command buffer
        const VkCommandBufferAllocateInfo allocInfo {
            .sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .commandPool        = mCommandPool,
            .level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };
        VULKAN_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &mCommandBuffer));
        ENGINE_ASSERT_MESSAGE(mCommandBuffer != VK_NULL_HANDLE, "VulkanFrame::Init — failed to allocate command buffer");

        // Semaphore
        constexpr VkSemaphoreCreateInfo semaphoreInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };
        VULKAN_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &mRenderFinishedSemaphore));
        ENGINE_ASSERT_MESSAGE(mRenderFinishedSemaphore != VK_NULL_HANDLE, "VulkanFrame::Init — failed to create semaphore");

        // Fence
        constexpr VkFenceCreateInfo fenceInfo {
            .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
            .flags = VK_FENCE_CREATE_SIGNALED_BIT,
        };
        VULKAN_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &mInFlightFence));
        ENGINE_ASSERT_MESSAGE(mInFlightFence != VK_NULL_HANDLE, "VulkanFrame::Init — failed to create fence");

        mInitialized = true;
        ENGINE_LOG_DEBUG("VulkanFrame", "VulkanFrame initialized");
    }

    void VulkanFrame::Destroy(VkDevice device) {
        if (!mInitialized) return;

        ENGINE_ASSERT_MESSAGE(device != VK_NULL_HANDLE, "VulkanFrame::Destroy — device is null");

        // Semaphore
        if (mRenderFinishedSemaphore != VK_NULL_HANDLE)
            vkDestroySemaphore(device, mRenderFinishedSemaphore, nullptr);

        // Fence
        if (mInFlightFence != VK_NULL_HANDLE)
            vkDestroyFence(device, mInFlightFence, nullptr);

        // Command pool
        if (mCommandPool != VK_NULL_HANDLE)
            vkDestroyCommandPool(device, mCommandPool, nullptr);

        mRenderFinishedSemaphore = VK_NULL_HANDLE;
        mInFlightFence           = VK_NULL_HANDLE;
        mCommandPool             = VK_NULL_HANDLE;
        mCommandBuffer           = VK_NULL_HANDLE;

        mInitialized = false;
        ENGINE_LOG_DEBUG("VulkanFrame", "VulkanFrame destroyed");
    }

    void VulkanFrame::BeginCommandBuffer() const {
        // Prepare command buffer for recording
        ENGINE_ASSERT_MESSAGE(mInitialized, "VulkanFrame::BeginCommandBuffer — not initialized");

        VULKAN_CHECK(vkResetCommandBuffer(mCommandBuffer, 0));

        constexpr VkCommandBufferBeginInfo beginInfo {
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
            .flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT,
        };
        VULKAN_CHECK(vkBeginCommandBuffer(mCommandBuffer, &beginInfo));
    }

    void VulkanFrame::EndCommandBuffer() const {
        // Stop recording into command buffer
        ENGINE_ASSERT_MESSAGE(mInitialized, "VulkanFrame::EndCommandBuffer — not initialized");
        VULKAN_CHECK(vkEndCommandBuffer(mCommandBuffer));
    }

} // Engine