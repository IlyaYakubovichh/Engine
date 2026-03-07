//
// Created by ilya on 07.03.2026.
//

#ifndef ENGINE_VULKANSWAPCHAIN_H
#define ENGINE_VULKANSWAPCHAIN_H

#include "Macro.h"
#include "Utility.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {

    struct VulkanSwapchainSpecification {
        VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
        VkDevice         device         { VK_NULL_HANDLE };
        VkSurfaceKHR     surface        { VK_NULL_HANDLE };
        uint32_t         width          { 0 };
        uint32_t         height         { 0 };
    };

    class ENGINE_API VulkanSwapchain final : public NonCopyable {
    public:
        VulkanSwapchain()  = default;
        ~VulkanSwapchain() override = default;

        void Initialize(const VulkanSwapchainSpecification& desc);
        void Destroy(VkDevice device);

        [[nodiscard]] uint32_t AcquireNextImage(VkDevice device, uint64_t timeout = UINT64_MAX) const;

        // Getters
        [[nodiscard]] VkSwapchainKHR                   GetSwapchain()     const { return mSwapchain;   }
        [[nodiscard]] VkFormat                         GetImageFormat()   const { return mImageFormat; }
        [[nodiscard]] VkExtent2D                       GetExtent()        const { return mExtent;      }
        [[nodiscard]] const std::vector<VkImage>&      GetImages()        const { return mImages;      }
        [[nodiscard]] const std::vector<VkImageView>&  GetImageViews()    const { return mImageViews;  }
        [[nodiscard]] uint32_t                         GetImageCount()    const { return static_cast<uint32_t>(mImages.size()); }

        // One semaphore per swapchain image — indexed by the imageIndex from AcquireNextImage
        [[nodiscard]] VkSemaphore GetImageAvailableSemaphore(uint32_t imageIndex) const;

    private:
        void CreateSwapchain(const VulkanSwapchainSpecification& desc);
        void CreateImageViews(VkDevice device);
        void CreateSemaphores(VkDevice device);

        VkSwapchainKHR           mSwapchain   { VK_NULL_HANDLE };
        VkFormat                 mImageFormat { VK_FORMAT_UNDEFINED };
        VkExtent2D               mExtent      {};

        std::vector<VkImage>     mImages;
        std::vector<VkImageView> mImageViews;

        // One per swapchain image — signals when that image is safe to render into
        std::vector<VkSemaphore> mImageAvailableSemaphores;

        bool mInitialized { false };
    };

} // Engine

#endif // ENGINE_VULKANSWAPCHAIN_H