#pragma once
#include "Macros.h"
#include "Subsystem.h"
#include "Vulkan/VulkanImage.h"
#include <vulkan/vulkan.h>

namespace Engine {

    /**
     * @brief Manages Vulkan synchronisation — pipeline barriers and image layout transitions.
     *
     * Uses VkImageMemoryBarrier2 (synchronisation2). Call Transition() to move a
     * VulkanImage to a new sync state; the image's stored state is updated automatically.
     * Call InsertBarrier() when you need a raw barrier without a managed image.
     */
    class ENGINE_API VulkanSyncSubsystem final : public VulkanSubsystem {
    public:
        VulkanSyncSubsystem() = default;
        ~VulkanSyncSubsystem() override = default;

        void Destroy() override;

        // ── Image transitions ─────────────────────────────────────────────────────

        // Transitions image to newState. Records a VkImageMemoryBarrier2 into cmd
        // and updates image.GetSyncState() to reflect the new state.
        void Transition(
            VkCommandBuffer             cmd,
            VulkanImage&                image,
            const VulkanImageSyncState& newState) const;

        // ── Raw barriers ──────────────────────────────────────────────────────────

        // Inserts an explicit image barrier built entirely by the caller.
        void InsertBarrier(
            VkCommandBuffer              cmd,
            const VkImageMemoryBarrier2& barrier) const;
    };

} // namespace Engine