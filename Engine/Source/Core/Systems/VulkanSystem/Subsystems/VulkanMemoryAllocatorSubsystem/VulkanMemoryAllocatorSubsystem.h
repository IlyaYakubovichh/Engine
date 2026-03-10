#ifndef ENGINE_VULKANMEMORYALLOCATORSUBSYSTEM_H
#define ENGINE_VULKANMEMORYALLOCATORSUBSYSTEM_H

#include "Macro.h"
#include "VulkanSystem/Subsystems/Subsystem.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Engine {

    // -------------------------------------------------------------------------
    // VulkanImageAllocation
    // Thin POD wrapper around a VMA-managed VkImage + its allocation metadata.
    // -------------------------------------------------------------------------
    struct ENGINE_API VulkanImageAllocation {
        VkImage           image      { VK_NULL_HANDLE };
        VmaAllocation     allocation { VK_NULL_HANDLE };
        VmaAllocationInfo info       {};

        [[nodiscard]] bool IsValid() const { return image != VK_NULL_HANDLE; }
    };

    // -------------------------------------------------------------------------
    // VulkanImageSpec
    // Everything VMA needs to allocate a VkImage.
    // -------------------------------------------------------------------------
    struct ENGINE_API VulkanImageSpec {
        VkImageCreateInfo        imageInfo       {};
        VmaMemoryUsage           memoryUsage     { VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE };
        VmaAllocationCreateFlags allocationFlags { 0 };
    };

    // -------------------------------------------------------------------------
    // VulkanMemoryAllocatorSubsystem
    // Thin subsystem around VMA. Owns the VmaAllocator lifetime.
    // All heavy memory operations (buffers, images) go through here.
    // -------------------------------------------------------------------------
    class ENGINE_API VulkanMemoryAllocatorSubsystem final : public Subsystem {
    public:
        VulkanMemoryAllocatorSubsystem()  = default;
        ~VulkanMemoryAllocatorSubsystem() override = default;

        void Initialize(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
        void Destroy() override;

        // Images
        [[nodiscard]] VulkanImageAllocation AllocateImage(const VulkanImageSpec& spec) const;
        void                                FreeImage(VulkanImageAllocation& allocation) const;

        // Debug
        void LogMemoryStats() const;

        [[nodiscard]] VmaAllocator GetAllocator() const { return mAllocator; }

    private:
        VmaAllocator mAllocator { VK_NULL_HANDLE };
    };

} // namespace Engine

#endif // ENGINE_VULKANMEMORYALLOCATORSUBSYSTEM_H