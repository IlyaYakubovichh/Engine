#include "VulkanSyncSubsystem.h"
#include "Log/LogSystem.h"

namespace Engine {

    void VulkanSyncSubsystem::Shutdown()
    {
        if (!IsInitialized()) return;

        MarkUninitialized();
        ENGINE_LOG_DEBUG("VulkanSyncSubsystem", "Shutdown");
    }

    void VulkanSyncSubsystem::Transition(
        VkCommandBuffer             cmd,
        VulkanImage&                image,
        const VulkanImageSyncState& newState) const
    {
        ENGINE_ASSERT_MSG(cmd != VK_NULL_HANDLE, "VulkanSyncSubsystem::Transition: null command buffer");
        ENGINE_ASSERT_MSG(image.GetVkImage() != VK_NULL_HANDLE, "VulkanSyncSubsystem::Transition: null image");

        const VulkanImageSyncState& old = image.GetSyncState();

        const VkImageMemoryBarrier2 barrier{
            .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2,
            .srcStageMask = old.stageMask,
            .srcAccessMask = old.accessMask,
            .dstStageMask = newState.stageMask,
            .dstAccessMask = newState.accessMask,
            .oldLayout = old.layout,
            .newLayout = newState.layout,
            .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
            .image = image.GetVkImage(),
            .subresourceRange = {
                .aspectMask = ToVkAspectFlags(image.GetFormat()),
                .baseMipLevel = 0,
                .levelCount = image.GetMipLevels(),
                .baseArrayLayer = 0,
                .layerCount = image.GetArrayLayers(),
            },
        };

        InsertBarrier(cmd, barrier);
        image.SetSyncState(newState);
    }

    void VulkanSyncSubsystem::InsertBarrier(
        VkCommandBuffer              cmd,
        const VkImageMemoryBarrier2& barrier) const
    {
        ENGINE_ASSERT_MSG(cmd != VK_NULL_HANDLE, "VulkanSyncSubsystem::InsertBarrier: null command buffer");

        const VkDependencyInfo depInfo{
            .sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO,
            .imageMemoryBarrierCount = 1,
            .pImageMemoryBarriers = &barrier,
        };

        vkCmdPipelineBarrier2(cmd, &depInfo);
    }

} // namespace Engine