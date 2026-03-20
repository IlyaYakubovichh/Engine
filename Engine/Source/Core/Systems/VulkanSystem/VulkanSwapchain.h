#pragma once

#include "Macros.h"
#include "Utils.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {

    // All inputs required to build or rebuild a swapchain.
    struct VulkanSwapchainSpec {
        VkPhysicalDevice physicalDevice{ VK_NULL_HANDLE };
        VkDevice         device{ VK_NULL_HANDLE };
        VkSurfaceKHR     surface{ VK_NULL_HANDLE };
        uint32_t         width{ 0 };
        uint32_t         height{ 0 };
    };

    /**
     * @brief RAII wrapper around a VkSwapchainKHR.
     *
     * Owns image views and per-image render-finished semaphores.
     * Call Initialize() once after construction, Destroy() before destruction.
     */
    class ENGINE_API VulkanSwapchain final : public NonCopyable {
    public:
        VulkanSwapchain() = default;
        ~VulkanSwapchain() override = default;

        void Initialize(const VulkanSwapchainSpec& spec);
        void Destroy(VkDevice device);

        // Acquires the next presentable image index. Blocks up to UINT64_MAX ns.
        [[nodiscard]] uint32_t AcquireNextImage(
            VkDevice    device,
            VkSemaphore imageAvailableSemaphore) const;

        // Returns the render-finished semaphore for the given swapchain image index.
        [[nodiscard]] VkSemaphore GetRenderFinishedSemaphore(uint32_t imageIndex) const;

        // ── Accessors ─────────────────────────────────────────────────────────────
        [[nodiscard]] VkSwapchainKHR                  GetSwapchain()   const { return mSwapchain;   }
        [[nodiscard]] VkFormat                        GetImageFormat() const { return mImageFormat; }
        [[nodiscard]] VkExtent2D                      GetExtent()      const { return mExtent;      }
        [[nodiscard]] const std::vector<VkImage>&     GetImages()      const { return mImages;      }
        [[nodiscard]] const std::vector<VkImageView>& GetImageViews()  const { return mImageViews;  }
        [[nodiscard]] uint32_t                        GetImageCount()  const
        {
            return static_cast<uint32_t>(mImages.size());
        }

    private:
        void CreateSwapchain(const VulkanSwapchainSpec& spec);
        void CreateImageViews(VkDevice device);
        void CreateSemaphores(VkDevice device);

        VkSwapchainKHR           mSwapchain{ VK_NULL_HANDLE };
        VkFormat                 mImageFormat{ VK_FORMAT_UNDEFINED };
        VkExtent2D               mExtent{};
        std::vector<VkImage>     mImages;
        std::vector<VkImageView> mImageViews;

        // One semaphore per swapchain image, indexed by AcquireNextImage result.
        std::vector<VkSemaphore> mRenderFinishedSemaphores;

        bool mInitialized{ false };
    };

} // namespace Engine