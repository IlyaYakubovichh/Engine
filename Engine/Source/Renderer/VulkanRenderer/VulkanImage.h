#pragma once
#include "Utility.h"
#include "Image.h"
#include "VulkanSystem/Subsystems/VulkanSyncSubsystem/VulkanSyncSubsystem.h"
#include "VulkanSystem/Subsystems/VulkanMemoryAllocatorSubsystem/VulkanMemoryAllocatorSubsystem.h"
#include <vulkan/vulkan.h>

namespace Engine {

	VkFormat            ToVkFormat(ImageFormat format);
	VkImageUsageFlags   ToVkImageUsage(ImageUsage usage);
	VkImageAspectFlags  ToVkAspectFlags(ImageFormat format);

	class ENGINE_API VulkanImage final : public Image {
	public:
		VulkanImage() = default;
		~VulkanImage() override = default;

		void Initialize(const ImageSpec& spec);
		void Free();

		uint32_t    GetWidth()       const override { return mSpec.width; }
		uint32_t    GetHeight()      const override { return mSpec.height; }
		uint32_t    GetMipLevels()   const override { return mSpec.mipLevels; }
		uint32_t    GetArrayLayers() const override { return mSpec.arrayLayers; }
		ImageFormat GetFormat()      const override { return mSpec.format; }
		ImageUsage  GetUsage()       const override { return mSpec.usage; }

		VkImage     GetVkImage()     const { return mAllocation.image; }
		VkImageView GetVkImageView() const { return mImageView; }
		VkFormat    GetVkFormat()    const { return ToVkFormat(mSpec.format); }

		VulkanImageSyncState& GetSyncState() { return mSyncState; }
		const VulkanImageSyncState& GetSyncState() const { return mSyncState; }

	private:
		void Allocate();
		void CreateView();

		ImageSpec               mSpec{};
		VulkanImageAllocation   mAllocation{};
		VkImageView             mImageView{ VK_NULL_HANDLE };
		VkDevice                mDevice{ VK_NULL_HANDLE };
		VulkanImageSyncState    mSyncState{};
	};

} // namespace Engine