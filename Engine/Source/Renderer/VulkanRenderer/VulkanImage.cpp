#include "VulkanImage.h"
#include "VulkanUtility.h"
#include "VulkanSystem/VulkanSystem.h"

namespace Engine {

    VkFormat ToVkFormat(ImageFormat format) {
        switch (format) {
            case ImageFormat::RGBA8_UNORM:       return VK_FORMAT_R8G8B8A8_UNORM;
            case ImageFormat::BGRA8_UNORM:       return VK_FORMAT_B8G8R8A8_UNORM;
            case ImageFormat::RGBA16_SFLOAT:     return VK_FORMAT_R16G16B16A16_SFLOAT;
            case ImageFormat::RGBA32_SFLOAT:     return VK_FORMAT_R32G32B32A32_SFLOAT;
            case ImageFormat::D32_SFLOAT:        return VK_FORMAT_D32_SFLOAT;
            case ImageFormat::D24_UNORM_S8_UINT: return VK_FORMAT_D24_UNORM_S8_UINT;
            default:                             return VK_FORMAT_UNDEFINED;
        }
    }

    VkImageUsageFlags ToVkImageUsage(ImageUsage usage) {
        VkImageUsageFlags flags = 0;
        if (HasFlag(usage, ImageUsage::RenderTarget)) flags |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (HasFlag(usage, ImageUsage::Sampled))      flags |= VK_IMAGE_USAGE_SAMPLED_BIT;
        if (HasFlag(usage, ImageUsage::DepthStencil)) flags |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
        if (HasFlag(usage, ImageUsage::Storage))      flags |= VK_IMAGE_USAGE_STORAGE_BIT;
        if (HasFlag(usage, ImageUsage::TransferSrc))  flags |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
        if (HasFlag(usage, ImageUsage::TransferDst))  flags |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        return flags;
    }

    VkImageAspectFlags ToVkAspectFlags(ImageFormat format) {
        switch (format) {
            case ImageFormat::D32_SFLOAT:        return VK_IMAGE_ASPECT_DEPTH_BIT;
            case ImageFormat::D24_UNORM_S8_UINT: return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
            default:                             return VK_IMAGE_ASPECT_COLOR_BIT;
        }
    }

	void VulkanImage::Initialize(const ImageSpec& spec) {
		ENGINE_ASSERT_MESSAGE(spec.width > 0 && spec.height > 0, "VulkanImage: invalid dimensions");
		ENGINE_ASSERT_MESSAGE(spec.format != ImageFormat::Undefined, "VulkanImage: undefined format");

		mSpec = spec;
		mDevice = VulkanSystem::GetInstance()->GetVkDevice();

		Allocate();
		CreateView();
	}

	void VulkanImage::Allocate() {
		auto vmaSubsystem = VulkanSystem::GetInstance()->GetMemoryAllocatorSubsystem();
		ENGINE_ASSERT_MESSAGE(vmaSubsystem && vmaSubsystem->IsInitialized(), "VulkanImage: VMA subsystem is not initialized");

		VkImageCreateInfo imageInfo{ VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO };
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = ToVkFormat(mSpec.format);
		imageInfo.extent = { mSpec.width, mSpec.height, 1 };
		imageInfo.mipLevels = mSpec.mipLevels;
		imageInfo.arrayLayers = mSpec.arrayLayers;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = ToVkImageUsage(mSpec.usage);
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VulkanImageSpec spec;
		spec.imageInfo = imageInfo;
		spec.memoryUsage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;
		spec.allocationFlags = VMA_ALLOCATION_CREATE_DEDICATED_MEMORY_BIT;

		mAllocation = vmaSubsystem->AllocateImage(spec);
		ENGINE_ASSERT_MESSAGE(mAllocation.image != VK_NULL_HANDLE, "VulkanImage: failed to allocate image");
	}

	void VulkanImage::CreateView() {
		VkImageViewCreateInfo viewInfo{ VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO };
		viewInfo.image = mAllocation.image;
		viewInfo.viewType = mSpec.arrayLayers > 1
			? VK_IMAGE_VIEW_TYPE_2D_ARRAY
			: VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = ToVkFormat(mSpec.format);

		viewInfo.subresourceRange.aspectMask = ToVkAspectFlags(mSpec.format);
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mSpec.mipLevels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = mSpec.arrayLayers;

		VULKAN_CHECK(vkCreateImageView(mDevice, &viewInfo, nullptr, &mImageView));
	}

	void VulkanImage::Free() {
		if (mDevice == VK_NULL_HANDLE)
			return;

		if (mImageView != VK_NULL_HANDLE) {
			vkDestroyImageView(mDevice, mImageView, nullptr);
			mImageView = VK_NULL_HANDLE;
		}

		if (mAllocation.image != VK_NULL_HANDLE) {
			auto* vmaSubsystem = VulkanSystem::GetInstance()->GetMemoryAllocatorSubsystem().get();
			vmaSubsystem->FreeImage(mAllocation);
		}
	}

} // namespace Engine