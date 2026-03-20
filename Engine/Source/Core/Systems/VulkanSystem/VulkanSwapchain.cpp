#include "VulkanSwapchain.h"
#include "Systems/LogSystem/LogSystem.h"
#include "VulkanUtils.h"
#include <VkBootstrap.h>

namespace Engine {

    // ─── Public ──────────────────────────────────────────────────────────────────

    void VulkanSwapchain::Initialize(const VulkanSwapchainSpec& spec)
    {
        ENGINE_ASSERT_MSG(!mInitialized, "VulkanSwapchain: already initialized");
        ENGINE_ASSERT_MSG(spec.device != VK_NULL_HANDLE, "VulkanSwapchain: device is null");
        ENGINE_ASSERT_MSG(spec.physicalDevice != VK_NULL_HANDLE, "VulkanSwapchain: physicalDevice is null");
        ENGINE_ASSERT_MSG(spec.surface != VK_NULL_HANDLE, "VulkanSwapchain: surface is null");

        CreateSwapchain(spec);
        CreateImageViews(spec.device);
        CreateSemaphores(spec.device);

        ENGINE_LOG_DEBUG("VulkanSwapchain", "Initialized ({}x{}, {} images)", mExtent.width, mExtent.height, mImages.size());
        mInitialized = true;
    }

    void VulkanSwapchain::Destroy(VkDevice device)
    {
        if (!mInitialized) return;
        ENGINE_ASSERT_MSG(device != VK_NULL_HANDLE, "VulkanSwapchain: device is null");

        for (const auto semaphore : mRenderFinishedSemaphores)
            vkDestroySemaphore(device, semaphore, nullptr);

        for (const auto view : mImageViews)
            vkDestroyImageView(device, view, nullptr);

        vkDestroySwapchainKHR(device, mSwapchain, nullptr);

        mRenderFinishedSemaphores.clear();
        mImageViews.clear();
        mImages.clear();
        mSwapchain = VK_NULL_HANDLE;
        mInitialized = false;

        ENGINE_LOG_DEBUG("VulkanSwapchain", "Destroyed");
    }

    uint32_t VulkanSwapchain::AcquireNextImage(
        VkDevice    device,
        VkSemaphore imageAvailableSemaphore) const
    {
        ENGINE_ASSERT_MSG(mInitialized, "VulkanSwapchain: not initialized");

        uint32_t imageIndex = 0;
        VK_CHECK(vkAcquireNextImageKHR(
            device, 
            mSwapchain,
            UINT64_MAX,
            imageAvailableSemaphore,
            VK_NULL_HANDLE,
            &imageIndex));

        return imageIndex;
    }

    VkSemaphore VulkanSwapchain::GetRenderFinishedSemaphore(const uint32_t imageIndex) const
    {
        ENGINE_ASSERT_MSG(imageIndex < mRenderFinishedSemaphores.size(), "VulkanSwapchain: semaphore index out of range");
        return mRenderFinishedSemaphores[imageIndex];
    }

    // ─── Private ─────────────────────────────────────────────────────────────────

    void VulkanSwapchain::CreateSwapchain(const VulkanSwapchainSpec& spec)
    {
        auto result = vkb::SwapchainBuilder{ spec.physicalDevice, spec.device, spec.surface }
            .set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // vsync
            .set_desired_extent(spec.width, spec.height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build();

        ENGINE_ASSERT_MSG(result.has_value(), "VulkanSwapchain: vkb build failed");

        auto& vkbSwapchain = result.value();
        mSwapchain = vkbSwapchain.swapchain;
        mImageFormat = vkbSwapchain.image_format;
        mExtent = vkbSwapchain.extent;
        mImages = vkbSwapchain.get_images().value();
    }

    void VulkanSwapchain::CreateImageViews(VkDevice device)
    {
        mImageViews.resize(mImages.size());

        for (size_t i = 0; i < mImages.size(); ++i) {
            const VkImageViewCreateInfo viewInfo{
                .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image = mImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format = mImageFormat,
                .subresourceRange = {
                    .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel = 0,
                    .levelCount = 1,
                    .baseArrayLayer = 0,
                    .layerCount = 1,
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

        for (auto& semaphore : mRenderFinishedSemaphores) {
            VK_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore));
            ENGINE_ASSERT_MSG(semaphore != VK_NULL_HANDLE, "VulkanSwapchain: semaphore creation failed");
        }
    }

} // namespace Engine