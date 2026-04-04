#include "VulkanSwapchain.h"
#include "Log/LogSystem.h"
#include "VulkanUtils.h"
#include <VkBootstrap.h>

namespace Engine {

    VulkanSwapchain::VulkanSwapchain(const VulkanSwapchainSettings& settings)
    {
        ENGINE_ASSERT_MSG(settings.device != VK_NULL_HANDLE, "VulkanSwapchain: device is null");
        ENGINE_ASSERT_MSG(settings.physicalDevice != VK_NULL_HANDLE, "VulkanSwapchain: physicalDevice is null");
        ENGINE_ASSERT_MSG(settings.surface != VK_NULL_HANDLE, "VulkanSwapchain: surface is null");

        CreateSwapchain(settings);
        CreateImageViews(settings.device);
        CreateSemaphores(settings.device);

        ENGINE_LOG_DEBUG("VulkanSwapchain", "Initialized ({}x{}, {} images)", mExtent.width, mExtent.height, mImages.size());
    }

    void VulkanSwapchain::Shutdown(VkDevice device)
    {
        ENGINE_ASSERT_MSG(device != VK_NULL_HANDLE, "VulkanSwapchain: device is null");

        for (auto s : mRenderFinishedSemaphores) vkDestroySemaphore(device, s, nullptr);
        for (auto v : mImageViews)               vkDestroyImageView(device, v, nullptr);
        vkDestroySwapchainKHR(device, mSwapchain, nullptr);

        mRenderFinishedSemaphores.clear();
        mImageViews.clear();
        mImages.clear();
        mSwapchain = VK_NULL_HANDLE;

        ENGINE_LOG_DEBUG("VulkanSwapchain", "Shutdown");
    }

    uint32_t VulkanSwapchain::AcquireNextImage(VkDevice device, VkSemaphore imageAvailableSemaphore) const
    {
        uint32_t imageIndex = 0;
        VK_CHECK(vkAcquireNextImageKHR(device, mSwapchain, UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE, &imageIndex));
        return imageIndex;
    }

    VkSemaphore VulkanSwapchain::GetRenderFinishedSemaphore(uint32_t imageIndex) const
    {
        ENGINE_ASSERT_MSG(imageIndex < mRenderFinishedSemaphores.size(), "VulkanSwapchain: semaphore index out of range");
        return mRenderFinishedSemaphores[imageIndex];
    }

    // ── Private ───────────────────────────────────────────────────────────────────

    void VulkanSwapchain::CreateSwapchain(const VulkanSwapchainSettings& settings)
    {
        auto result = vkb::SwapchainBuilder{ settings.physicalDevice, settings.device, settings.surface }
            .set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
            .set_desired_extent(settings.width, settings.height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build();

        ENGINE_ASSERT_MSG(result.has_value(), "VulkanSwapchain: vkb build failed");

        auto& vkbSwapchain  = result.value();
        mSwapchain          = vkbSwapchain.swapchain;
        mImageFormat        = vkbSwapchain.image_format;
        mExtent             = vkbSwapchain.extent;
        mImages             = vkbSwapchain.get_images().value();
    }

    void VulkanSwapchain::CreateImageViews(VkDevice device)
    {
        mImageViews.resize(mImages.size());

        for (size_t i = 0; i < mImages.size(); ++i) {
            const VkImageViewCreateInfo viewInfo{
                .sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image      = mImages[i],
                .viewType   = VK_IMAGE_VIEW_TYPE_2D,
                .format     = mImageFormat,
                .subresourceRange = {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
            };
            VK_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &mImageViews[i]));
        }
    }

    void VulkanSwapchain::CreateSemaphores(VkDevice device)
    {
        mRenderFinishedSemaphores.resize(mImages.size(), VK_NULL_HANDLE);

        constexpr VkSemaphoreCreateInfo semaphoreInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        for (auto& semaphore : mRenderFinishedSemaphores)
            VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore));
    }

} // namespace Engine