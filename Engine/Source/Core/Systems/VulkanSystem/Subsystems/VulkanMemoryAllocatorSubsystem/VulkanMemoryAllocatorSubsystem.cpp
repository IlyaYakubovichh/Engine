#include "VulkanMemoryAllocatorSubsystem.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace Engine {

    void VulkanMemoryAllocatorSubsystem::Initialize(
        VkInstance       instance,
        VkPhysicalDevice physicalDevice,
        VkDevice         device)
    {
        ENGINE_ASSERT_MESSAGE(!IsInitialized(), "VulkanMemoryAllocatorSubsystem::Initialize — already initialized");

        const VmaAllocatorCreateInfo allocatorInfo {
            .flags            = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT,
            .physicalDevice   = physicalDevice,
            .device           = device,
            .instance         = instance,
            .vulkanApiVersion = VK_API_VERSION_1_3,
        };

        VULKAN_CHECK(vmaCreateAllocator(&allocatorInfo, &mAllocator));
        MarkInitialized();
        ENGINE_LOG_DEBUG("VulkanMemoryAllocator", "VmaAllocator created successfully");
    }

    void VulkanMemoryAllocatorSubsystem::Destroy() {
        if (!IsInitialized()) return;

        vmaDestroyAllocator(mAllocator);
        mAllocator = VK_NULL_HANDLE;
        MarkUninitialized();

        ENGINE_LOG_DEBUG("VulkanMemoryAllocator", "VmaAllocator destroyed");
    }

    VulkanImageAllocation VulkanMemoryAllocatorSubsystem::AllocateImage(const VulkanImageSpec& spec) const {
        ENGINE_ASSERT_MESSAGE(IsInitialized(), "VulkanMemoryAllocatorSubsystem::AllocateImage — not initialized");

        const VmaAllocationCreateInfo allocInfo {
            .flags = spec.allocationFlags,
            .usage = spec.memoryUsage,
        };

        VulkanImageAllocation result {};
        VULKAN_CHECK(vmaCreateImage(
            mAllocator,
            &spec.imageInfo,
            &allocInfo,
            &result.image,
            &result.allocation,
            &result.info
        ));

        return result;
    }

    void VulkanMemoryAllocatorSubsystem::FreeImage(VulkanImageAllocation& allocation) const {
        if (!allocation.IsValid()) return;

        vmaDestroyImage(mAllocator, allocation.image, allocation.allocation);

        allocation.image      = VK_NULL_HANDLE;
        allocation.allocation = VK_NULL_HANDLE;
        allocation.info       = {};
    }

    void VulkanMemoryAllocatorSubsystem::LogMemoryStats() const {
        if (!IsInitialized()) return;

        char* statsString = nullptr;
        vmaBuildStatsString(mAllocator, &statsString, VK_TRUE);

        if (statsString) {
            ENGINE_LOG_DEBUG("VulkanMemoryAllocator", "Memory stats:\n{}", statsString);
            vmaFreeStatsString(mAllocator, statsString);
        }
    }

} // namespace Engine