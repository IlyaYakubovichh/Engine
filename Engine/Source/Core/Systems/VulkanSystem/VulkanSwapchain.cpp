//
// Created by ilya on 07.03.2026.
//

#include "VulkanSwapchain.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"
#include <VkBootstrap.h>

namespace Engine {

    void VulkanSwapchain::Initialize(const VulkanSwapchainSpecification& desc) {
        ENGINE_ASSERT_MESSAGE(!mInitialized,      "VulkanSwapchain::Init — already initialized");
        ENGINE_ASSERT_MESSAGE(desc.device         != VK_NULL_HANDLE, "VulkanSwapchain::Init — device is null");
        ENGINE_ASSERT_MESSAGE(desc.physicalDevice != VK_NULL_HANDLE, "VulkanSwapchain::Init — physicalDevice is null");
        ENGINE_ASSERT_MESSAGE(desc.surface        != VK_NULL_HANDLE, "VulkanSwapchain::Init — surface is null");

        CreateSwapchain(desc);
        CreateImageViews(desc.device);
        CreateSemaphores(desc.device);

        ENGINE_LOG_DEBUG("VulkanSwapchain", "Swapchain initialized ({}x{}, {} images)", mExtent.width, mExtent.height, mImages.size());
        mInitialized = true;
    }

    void VulkanSwapchain::Destroy(VkDevice device) {
        if (!mInitialized) return;

        ENGINE_ASSERT_MESSAGE(device != VK_NULL_HANDLE, "VulkanSwapchain::Destroy — device is null");

        for (const auto semaphore : mImageAvailableSemaphores)
            vkDestroySemaphore(device, semaphore, nullptr);

        for (const auto view : mImageViews)
            vkDestroyImageView(device, view, nullptr);

        vkDestroySwapchainKHR(device, mSwapchain, nullptr);

        mImageAvailableSemaphores.clear();
        mImageViews.clear();
        mImages.clear();
        mSwapchain   = VK_NULL_HANDLE;
        mInitialized = false;

        ENGINE_LOG_DEBUG("VulkanSwapchain", "Swapchain destroyed");
    }

    uint32_t VulkanSwapchain::AcquireNextImage(VkDevice device, const uint64_t timeout) const {
        ENGINE_ASSERT_MESSAGE(mInitialized, "VulkanSwapchain::AcquireNextImage — not initialized");

        uint32_t imageIndex = 0;

        VULKAN_CHECK(vkAcquireNextImageKHR(
            device,
            mSwapchain,
            timeout,
            mImageAvailableSemaphores[imageIndex],
            VK_NULL_HANDLE,
            &imageIndex
        ));

        return imageIndex;
    }

    VkSemaphore VulkanSwapchain::GetImageAvailableSemaphore(const uint32_t imageIndex) const {
        ENGINE_ASSERT_MESSAGE(imageIndex < mImageAvailableSemaphores.size(), "VulkanSwapchain::GetImageAvailableSemaphore — index out of range");
        return mImageAvailableSemaphores[imageIndex];
    }

    // -------------------------------------------------------------------------
    // Private
    // -------------------------------------------------------------------------
    void VulkanSwapchain::CreateSwapchain(const VulkanSwapchainSpecification& desc) {
        auto result = vkb::SwapchainBuilder{ desc.physicalDevice, desc.device, desc.surface }
            .set_desired_format({ VK_FORMAT_B8G8R8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }) // gamma corrected color space
            .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR) // v-sync
            .set_desired_extent(desc.width, desc.height)
            .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
            .build();

        ENGINE_ASSERT_MESSAGE(result.has_value(), "VulkanSwapchain: failed to build swapchain");

        auto& vkbSwapchain = result.value();
        mSwapchain         = vkbSwapchain.swapchain;
        mImageFormat       = vkbSwapchain.image_format;
        mExtent            = vkbSwapchain.extent;
        mImages            = vkbSwapchain.get_images().value();
    }

    void VulkanSwapchain::CreateImageViews(VkDevice device) {
        mImageViews.resize(mImages.size());

        for (size_t i = 0; i < mImages.size(); ++i) {
            VkImageViewCreateInfo viewInfo {
                .sType    = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
                .image    = mImages[i],
                .viewType = VK_IMAGE_VIEW_TYPE_2D,
                .format   = mImageFormat,
                .subresourceRange = {
                    .aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT,
                    .baseMipLevel   = 0,
                    .levelCount     = 1,
                    .baseArrayLayer = 0,
                    .layerCount     = 1,
                },
            };
            VULKAN_CHECK(vkCreateImageView(device, &viewInfo, nullptr, &mImageViews[i]));
        }
    }

    void VulkanSwapchain::CreateSemaphores(VkDevice device) {
        mImageAvailableSemaphores.resize(mImages.size(), VK_NULL_HANDLE);

        constexpr VkSemaphoreCreateInfo semaphoreInfo {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
        };

        for (auto& semaphore : mImageAvailableSemaphores) {
            VULKAN_CHECK(vkCreateSemaphore(device, &semaphoreInfo, nullptr, &semaphore));
            ENGINE_ASSERT_MESSAGE(semaphore != VK_NULL_HANDLE, "VulkanSwapchain: failed to create imageAvailable semaphore");
        }
    }

} // Engine