#pragma once
#include "Macros.h"
#include "Subsystem.h"
#include "Resources/VulkanImage.h"
#include <vulkan/vulkan.h>

namespace Engine {

    class ENGINE_API VulkanSyncSubsystem final : public VulkanSubsystem {
    public:
        VulkanSyncSubsystem() = default;
        ~VulkanSyncSubsystem() override = default;

        void Shutdown() override;

        void Transition(
            VkCommandBuffer             cmd,
            VulkanImage&                image,
            const VulkanImageSyncState& newState) const;

        void InsertBarrier(
            VkCommandBuffer              cmd,
            const VkImageMemoryBarrier2& barrier) const;
    };

} // namespace Engine