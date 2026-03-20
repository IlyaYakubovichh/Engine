#pragma once
#include "Macros.h"
#include "Renderer/Image.h"
#include "Systems/VulkanSystem/Subsystems/VulkanMemAllocSubsystem.h"
#include <vulkan/vulkan.h>

namespace Engine {

    // ─── VulkanImageSyncState ─────────────────────────────────────────────────────

    /// Tracks the current pipeline sync state of a VulkanImage.
    struct VulkanImageSyncState {
        VkPipelineStageFlags2 stageMask{ VK_PIPELINE_STAGE_2_NONE };
        VkAccessFlags2        accessMask{ VK_ACCESS_2_NONE };
        VkImageLayout         layout{ VK_IMAGE_LAYOUT_UNDEFINED };
    };

    // ─── Format / usage conversion helpers ───────────────────────────────────────

    [[nodiscard]] VkFormat           ToVkFormat(ImageFormat format);
    [[nodiscard]] VkImageUsageFlags  ToVkImageUsage(ImageUsage usage);
    [[nodiscard]] VkImageAspectFlags ToVkAspectFlags(ImageFormat format);

    // ─── VulkanImage ──────────────────────────────────────────────────────────────

    /**
     * @brief Vulkan backend implementation of Image.
     *
     * GPU memory is allocated via VMA on construction and released on Free() / destruction.
     */
    class ENGINE_API VulkanImage final : public Image {
    public:
        // Allocates GPU memory and creates the image view.
        explicit VulkanImage(const ImageSpec& spec);

        // Calls Free() if resources were not released explicitly.
        ~VulkanImage() override { Free(); }

        // Explicitly releases the image view and GPU memory. Safe to call multiple times.
        void Free() override;

        // ── Image interface ───────────────────────────────────────────────────────
        [[nodiscard]] uint32_t    GetWidth()       const override { return mSpec.width;         }
        [[nodiscard]] uint32_t    GetHeight()      const override { return mSpec.height;        }
        [[nodiscard]] uint32_t    GetMipLevels()   const override { return mSpec.mipLevels;     }
        [[nodiscard]] uint32_t    GetArrayLayers() const override { return mSpec.arrayLayers;   }
        [[nodiscard]] ImageFormat GetFormat()      const override { return mSpec.format;        }
        [[nodiscard]] ImageUsage  GetUsage()       const override { return mSpec.usage;         }

        // ── Vulkan-specific accessors ─────────────────────────────────────────────
        [[nodiscard]] VkImage     GetVkImage()     const { return mAllocation.image;        }
        [[nodiscard]] VkImageView GetVkImageView() const { return mImageView;               }
        [[nodiscard]] VkFormat    GetVkFormat()    const { return ToVkFormat(mSpec.format); }

        [[nodiscard]] VulkanImageSyncState& GetSyncState()              { return mSyncState;    }
        [[nodiscard]] const VulkanImageSyncState& GetSyncState() const  { return mSyncState;    }
        void SetSyncState(const VulkanImageSyncState& state)            { mSyncState = state;   }

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