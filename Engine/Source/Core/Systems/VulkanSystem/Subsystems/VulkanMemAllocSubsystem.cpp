#include "VulkanMemAllocSubsystem.h"
#include "Systems/LogSystem/LogSystem.h"
#include "VulkanUtils.h"

// VMA implementation unit — compiled exactly once.
#define VMA_IMPLEMENTATION
#include <vk_mem_alloc.h>

namespace Engine {

    void VulkanMemAllocSubsystem::Initialize(
        VkInstance       instance,
        VkPhysicalDevice physicalDevice,
        VkDevice         device)
    {
        ENGINE_ASSERT_MSG(!IsInitialized(), "VulkanMemAllocSubsystem: already initialized");

        const VmaAllocatorCreateInfo allocatorInfo{
            .flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice = physicalDevice,
            .device = device,
            .instance = instance,
            .vulkanApiVersion = VK_API_VERSION_1_3,
        };

        VK_CHECK(vmaCreateAllocator(&allocatorInfo, &mAllocator));
        MarkInitialized();

        ENGINE_LOG_DEBUG("VulkanMemAlloc", "VmaAllocator created");
    }

    void VulkanMemAllocSubsystem::Destroy()
    {
        if (!IsInitialized()) return;

        vmaDestroyAllocator(mAllocator);
        mAllocator = VK_NULL_HANDLE;
        MarkUninitialized();

        ENGINE_LOG_DEBUG("VulkanMemAlloc", "VmaAllocator destroyed");
    }

    VulkanImageAllocation VulkanMemAllocSubsystem::AllocateImage(const VulkanImageSpec& spec) const
    {
        ENGINE_ASSERT_MSG(IsInitialized(), "VulkanMemAllocSubsystem: not initialized");

        const VmaAllocationCreateInfo allocInfo{
            .flags = spec.allocationFlags,
            .usage = spec.memoryUsage,
        };

        VulkanImageAllocation result{};
        VK_CHECK(vmaCreateImage(mAllocator,
            &spec.imageInfo,
            &allocInfo,
            &result.image,
            &result.allocation,
            &result.info));

        return result;
    }

    void VulkanMemAllocSubsystem::FreeImage(VulkanImageAllocation& allocation) const
    {
        if (!allocation.IsValid()) return;

        vmaDestroyImage(mAllocator, allocation.image, allocation.allocation);
        allocation = {};
    }

    void VulkanMemAllocSubsystem::LogMemoryStats() const
    {
        if (!IsInitialized()) return;

        char* stats = nullptr;
        vmaBuildStatsString(mAllocator, &stats, VK_TRUE);

        if (stats) {
            ENGINE_LOG_DEBUG("VulkanMemAlloc", "Memory stats:\n{}", stats);
            vmaFreeStatsString(mAllocator, stats);
        }
    }

} // namespace Engine