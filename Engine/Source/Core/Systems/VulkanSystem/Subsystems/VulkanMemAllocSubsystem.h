#pragma once
#include "Macros.h"
#include "Subsystem.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Engine {

    /**
     * @brief POD handle for a VMA managed VkImage.
     *
     * Owns no lifetime, FreeImage() must be called explicitly
     * before the allocator is destroyed.
     */
    struct ENGINE_API VulkanImageAllocation {
        VkImage           image{ VK_NULL_HANDLE };
        VmaAllocation     allocation{ VK_NULL_HANDLE };
        VmaAllocationInfo info{};

        [[nodiscard]] bool IsValid() const { return image != VK_NULL_HANDLE; }
    };

    /// All inputs required to allocate a VkImage through VMA.
    struct ENGINE_API VulkanImageSpec {
        VkImageCreateInfo        imageInfo{};
        VmaMemoryUsage           memoryUsage{ VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE };
        VmaAllocationCreateFlags allocationFlags{ 0 };
    };

    /**
     * @brief Thin subsystem wrapper around VmaAllocator.
     *
     * Owns the VmaAllocator lifetime. All GPU memory operations
     * (images, buffers) are routed through here.
     */
    class ENGINE_API VulkanMemAllocSubsystem final : public VulkanSubsystem {
    public:
        VulkanMemAllocSubsystem(
            VkInstance       instance,
            VkPhysicalDevice physicalDevice,
            VkDevice         device);

        ~VulkanMemAllocSubsystem() override = default;

        void Destroy() override;

        // Allocates a VkImage via VMA. Asserts if not initialized.
        [[nodiscard]] VulkanImageAllocation AllocateImage(const VulkanImageSpec& spec) const;

        // Frees a VMA image and resets the allocation handle.
        void FreeImage(VulkanImageAllocation& allocation) const;

        // Dumps VMA memory budget and usage to the debug log.
        void LogMemoryStats() const;

        [[nodiscard]] VmaAllocator GetAllocator() const { return mAllocator; }

    private:
        VmaAllocator mAllocator{ VK_NULL_HANDLE };
    };

} // namespace Engine