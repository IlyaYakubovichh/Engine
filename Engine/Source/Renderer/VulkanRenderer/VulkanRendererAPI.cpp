#include "VulkanRendererAPI.h"
#include "VulkanSystem/VulkanSystem.h"
#include "WindowSystem/WindowSystem.h"
#include "LogSystem/LogSystem.h"
#include "VulkanUtility.h"

namespace Engine {

    static constexpr std::string_view LOG_CATEGORY = "VulkanRendererAPI";

    VulkanRendererAPI::VulkanRendererAPI() {
        mAPI = API::Vulkan;
    }

    void VulkanRendererAPI::Initialize() {
        auto* vulkan = VulkanSystem::GetInstance();

        mData.device        = vulkan->GetVkDevice();
        mData.graphicsQueue = vulkan->GetVkGraphicsQueue();
        mData.presentQueue  = vulkan->GetVkPresentationQueue();
        mData.swapchain     = vulkan->GetWindowContext(gMainWindowId).swapchain;

        ENGINE_ASSERT_MESSAGE(mData.device        != VK_NULL_HANDLE, "VulkanRendererAPI: device is null");
        ENGINE_ASSERT_MESSAGE(mData.graphicsQueue != VK_NULL_HANDLE, "VulkanRendererAPI: graphics queue is null");
        ENGINE_ASSERT_MESSAGE(mData.swapchain     != nullptr,        "VulkanRendererAPI: swapchain is null");

        const uint32_t graphicsQueueIndex = vulkan->GetVkGraphicsQueueIndex();
        for (auto& frame : mData.frames)
            frame.Initialize(mData.device, graphicsQueueIndex);

        ENGINE_LOG_INFO(LOG_CATEGORY, "Initialized ({} frames in flight)", MAX_FRAMES_IN_FLIGHT);
    }

    void VulkanRendererAPI::Shutdown() {
        for (auto& frame : mData.frames)
            frame.Destroy(mData.device);

        ENGINE_LOG_INFO(LOG_CATEGORY, "Shutdown");
    }

    void VulkanRendererAPI::BeginFrame() {
        auto& frame = mData.CurrentFrame();
    }

    void VulkanRendererAPI::EndFrame() {
        auto& frame = mData.CurrentFrame();
    }

    void VulkanRendererAPI::BeginRenderPass() {
        auto& frame = mData.CurrentFrame();
    }

    void VulkanRendererAPI::EndRenderPass() {
        auto& frame = mData.CurrentFrame();
    }

    void VulkanRendererAPI::Present() {
        mData.currentFrameIndex = (mData.currentFrameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

} // Engine