#include "VulkanImage.h"
#include "VulkanUtils.h"
#include "Vulkan/VulkanSystem.h"

namespace Engine {

    VkFormat ToVkFormat(const ImageFormat format)
    {
        switch (format) {
        case ImageFormat::RGBA8_Unorm:       return VK_FORMAT_R8G8B8A8_UNORM;
        case ImageFormat::BGRA8_Unorm:       return VK_FORMAT_B8G8R8A8_UNORM;
        case ImageFormat::RGBA16_Sfloat:     return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat::RGBA32_Sfloat:     return VK_FORMAT_R32G32B32A32_SFLOAT;
        case ImageFormat::D32_Sfloat:        return VK_FORMAT_D32_SFLOAT;
        case ImageFormat::D24_Unorm_S8_Uint: return VK_FORMAT_D24_UNORM_S8_UINT;
        default:                             return VK_FORMAT_UNDEFINED;
        }
    }

    VkImageUsageFlags ToVkImageUsage(const ImageUsage usage)
    {
        VkImageUsageFlags flags = 0;
        if (HasFlag(usage, ImageUsage::RenderTarget))
            flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
            | VK_IMAGE_USAGE_TRANSFER_SRC_BIT
            | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        if (HasFlag(usage, ImageUsage::Sampled))      flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (HasFlag(usage, ImageUsage::DepthStencil)) flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (HasFlag(usage, ImageUsage::Storage))      flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if (HasFlag(usage, ImageUsage::TransferSrc))  flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (HasFlag(usage, ImageUsage::TransferDst))  flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        return flags;
    }

    VkImageAspectFlags ToVkAspectFlags(const ImageFormat format)
    {
        switch (format) {
        case ImageFormat::D32_Sfloat:        return VK_IMAGE_ASPECT_DEPTH_BIT;
        case ImageFormat::D24_Unorm_S8_Uint: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        default:                             return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

    VulkanImage::VulkanImage(const ImageSpec& spec)
    {
        ENGINE_ASSERT_MSG(spec.width > 0 && spec.height > 0, "VulkanImage: invalid dimensions");
        ENGINE_ASSERT_MSG(spec.format != ImageFormat::Undefined, "VulkanImage: undefined format");

        mSpec = spec;
        mDevice = VulkanSystem::GetInstance()->GetVkDevice();

        Allocate();
        CreateView();
    }

    void VulkanImage::Free()
    {
        if (mDevice == VK_NULL_HANDLE) return;

        if (mImageView != VK_NULL_HANDLE) {
            vkDestroyImageView(mDevice, mImageView, nullptr);
            mImageView = VK_NULL_HANDLE;
        }

        if (mAllocation.IsValid())
            VulkanSystem::GetInstance()->GetMemAllocSubsystem()->FreeImage(mAllocation);

        mDevice = VK_NULL_HANDLE;
    }

    void VulkanImage::Allocate()
    {
        auto vma = VulkanSystem::GetInstance()->GetMemAllocSubsystem();
        ENGINE_ASSERT_MSG(vma && vma->IsInitialized(), "VulkanImage: VMA not initialized");

        // VulkanImageSettings — правильное имя структуры из VulkanMemAllocSubsystem.h
        const VulkanImageSettings settings{
            .imageInfo = {
                .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
                .imageType = VK_IMAGE_TYPE_2D,
                .format = ToVkFormat(mSpec.format),
                .extent = { mSpec.width, mSpec.height, 1 },
                .mipLevels = mSpec.mipLevels,
                .arrayLayers = mSpec.arrayLayers,
                .samples = VK_SAMPLE_COUNT_1_BIT,
                .tiling = VK_IMAGE_TILING_OPTIMAL,
                .usage = ToVkImageUsage(mSpec.usage),
                .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
                .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
            },
            .memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE,
            .allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT,
        };

        mAllocation = vma->AllocateImage(settings);
        ENGINE_ASSERT_MSG(mAllocation.IsValid(), "VulkanImage: allocation failed");
    }

    void VulkanImage::CreateView()
    {
        const VkImageViewCreateInfo viewInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .image = mAllocation.image,
            .viewType = mSpec.arrayLayers > 1
                            ? VK_IMAGE_VIEW_TYPE_2D_ARRAY
                            : VK_IMAGE_VIEW_TYPE_2D,
            .format = ToVkFormat(mSpec.format),
            .subresourceRange = {
                .aspectMask = ToVkAspectFlags(mSpec.format),
                .baseMipLevel = 0,
                .levelCount = mSpec.mipLevels,
                .baseArrayLayer = 0,
                .layerCount = mSpec.arrayLayers,
            },
        };
        VK_CHECK(vkCreateImageView(mDevice, &viewInfo, nullptr, &mImageView));
    }

} // namespace Engine