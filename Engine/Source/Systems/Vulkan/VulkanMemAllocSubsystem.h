#pragma once
#include "Macros.h"
#include "Subsystem.h"
#include <vulkan/vulkan.h>
#include <vk_mem_alloc.h>

namespace Engine {

    struct ENGINE_API VulkanImageAllocation {
        VkImage           image{ VK_NULL_HANDLE };
        VmaAllocation     allocation{ VK_NULL_HANDLE };
        VmaAllocationInfo info{};

        [[nodiscard]] bool IsValid() const { return image != VK_NULL_HANDLE; }
    };

    struct ENGINE_API VulkanImageSettings {
        VkImageCreateInfo        imageInfo{};
        VmaMemoryUsage           memoryUsage{ VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE };
        VmaAllocationCreateFlags allocationFlags{ 0 };
    };

    class ENGINE_API VulkanMemAllocSubsystem final : public VulkanSubsystem {
    public:
        explicit VulkanMemAllocSubsystem(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
        ~VulkanMemAllocSubsystem() override = default;

        void Shutdown() override;

        [[nodiscard]] VulkanImageAllocation AllocateImage(const VulkanImageSettings& settings) const;
        void                                FreeImage(VulkanImageAllocation& allocation) const;

        void LogMemoryStats() const;

        [[nodiscard]] VmaAllocator GetAllocator() const { return mAllocator; }

    private:

        VmaAllocator mAllocator{ VK_NULL_HANDLE };
    };

} // namespace Engine