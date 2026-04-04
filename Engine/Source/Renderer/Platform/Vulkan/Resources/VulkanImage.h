#pragma once
#include "Macros.h"
#include "Resources/Image.h"
#include "Vulkan/VulkanMemAllocSubsystem.h"
#include <vulkan/vulkan.h>

namespace Engine {

    struct VulkanImageSyncState {
        VkPipelineStageFlags2 stageMask{ VK_PIPELINE_STAGE_2_NONE };
        VkAccessFlags2        accessMask{ VK_ACCESS_2_NONE };
        VkImageLayout         layout{ VK_IMAGE_LAYOUT_UNDEFINED };
    };

    [[nodiscard]] VkFormat           ToVkFormat(ImageFormat format);
    [[nodiscard]] VkImageUsageFlags  ToVkImageUsage(ImageUsage  usage);
    [[nodiscard]] VkImageAspectFlags ToVkAspectFlags(ImageFormat format);

    class ENGINE_API VulkanImage final : public Image {
    public:
        explicit VulkanImage(const ImageSpec& spec);
        ~VulkanImage() override { Free(); }

        void Free() override;

        [[nodiscard]] uint32_t    GetWidth()       const override { return mSpec.width;         }
        [[nodiscard]] uint32_t    GetHeight()      const override { return mSpec.height;        }
        [[nodiscard]] uint32_t    GetMipLevels()   const override { return mSpec.mipLevels;     }
        [[nodiscard]] uint32_t    GetArrayLayers() const override { return mSpec.arrayLayers;   }
        [[nodiscard]] ImageFormat GetFormat()      const override { return mSpec.format;        }
        [[nodiscard]] ImageUsage  GetUsage()       const override { return mSpec.usage;         }

        [[nodiscard]] VkImage     GetVkImage()     const { return mAllocation.image;        }
        [[nodiscard]] VkImageView GetVkImageView() const { return mImageView;               }
        [[nodiscard]] VkFormat    GetVkFormat()    const { return ToVkFormat(mSpec.format); }

        [[nodiscard]] VulkanImageSyncState& GetSyncState()              { return mSyncState;  }
        [[nodiscard]] const VulkanImageSyncState& GetSyncState() const  { return mSyncState;  }
        void SetSyncState(const VulkanImageSyncState& state)            { mSyncState = state; }

    private:
        void Allocate();
        void CreateView();

        ImageSpec             mSpec{};
        VulkanImageAllocation mAllocation{};
        VkImageView           mImageView{ VK_NULL_HANDLE };
        VkDevice              mDevice{ VK_NULL_HANDLE };
        VulkanImageSyncState  mSyncState{};
    };

} // namespace Engine