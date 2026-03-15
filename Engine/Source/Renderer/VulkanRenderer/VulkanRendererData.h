#pragma once

#include "Macro.h"
#include "VulkanFrame.h"
#include "Utility.h"
#include "VulkanSystem/VulkanSwapchain.h"
#include <array>
#include <vulkan/vulkan.h>

namespace Engine {

	static constexpr uint32_t MAX_FRAMES_IN_FLIGHT = 3;

	struct ENGINE_API VulkanRendererData {
		VkDevice             device{ VK_NULL_HANDLE };
		VkQueue              graphicsQueue{ VK_NULL_HANDLE };
		VkQueue              presentQueue{ VK_NULL_HANDLE };
		Ref<VulkanSwapchain> swapchain{ nullptr };

		std::array<VulkanFrame, MAX_FRAMES_IN_FLIGHT> frames;
		uint32_t currentFrameIndex{ 0 };
		uint32_t imageIndex{ 0 };

		VkImage    activeRenderImage{ VK_NULL_HANDLE };
		VkExtent2D activeRenderExtent{ 0, 0 };

		[[nodiscard]] VulkanFrame& CurrentFrame() { return frames[currentFrameIndex]; }
		[[nodiscard]] const VulkanFrame& CurrentFrame() const { return frames[currentFrameIndex]; }

		[[nodiscard]] VkCommandBuffer CurrentCommandBuffer()           const { return frames[currentFrameIndex].GetCommandBuffer(); }
		[[nodiscard]] VkSemaphore     CurrentRenderFinishedSemaphore() const { return swapchain->GetRenderFinishedSemaphore(imageIndex); }
		[[nodiscard]] VkFence         CurrentInFlightFence()           const { return frames[currentFrameIndex].GetInFlightFence(); }
	};

} // namespace Engine