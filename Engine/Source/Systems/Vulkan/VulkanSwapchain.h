#pragma once
#include "Macros.h"
#include "Utils.h"
#include <vulkan/vulkan.h>
#include <vector>

namespace Engine {

    struct VulkanSwapchainSettings {
        VkPhysicalDevice physicalDevice { VK_NULL_HANDLE };
        VkDevice         device         { VK_NULL_HANDLE };
        VkSurfaceKHR     surface        { VK_NULL_HANDLE };
        uint32_t         width          { 0 };
        uint32_t         height         { 0 };
    };

    class ENGINE_API VulkanSwapchain final : public NonCopyable {
    public:
        explicit VulkanSwapchain(const VulkanSwapchainSettings& settings);
        ~VulkanSwapchain() override = default;

        void Shutdown(VkDevice device);

        [[nodiscard]] uint32_t    AcquireNextImage(VkDevice device, VkSemaphore imageAvailableSemaphore) const;
        [[nodiscard]] VkSemaphore GetRenderFinishedSemaphore(uint32_t imageIndex) const;

        [[nodiscard]] VkSwapchainKHR                  GetSwapchain()   const { return mSwapchain;                               }
        [[nodiscard]] VkFormat                        GetImageFormat() const { return mImageFormat;                             }
        [[nodiscard]] VkExtent2D                      GetExtent()      const { return mExtent;                                  }
        [[nodiscard]] const std::vector<VkImage>&     GetImages()      const { return mImages;                                  }
        [[nodiscard]] const std::vector<VkImageView>& GetImageViews()  const { return mImageViews;                              }
        [[nodiscard]] uint32_t                        GetImageCount()  const { return static_cast<uint32_t>(mImages.size());    }

    private:

        void CreateSwapchain(const VulkanSwapchainSettings& settings);
        void CreateImageViews(VkDevice device);
        void CreateSemaphores(VkDevice device);

        VkSwapchainKHR           mSwapchain     { VK_NULL_HANDLE };
        VkFormat                 mImageFormat   { VK_FORMAT_UNDEFINED };
        VkExtent2D               mExtent{};
        std::vector<VkImage>     mImages;
        std::vector<VkImageView> mImageViews;
        std::vector<VkSemaphore> mRenderFinishedSemaphores;
    };

} // namespace Engine