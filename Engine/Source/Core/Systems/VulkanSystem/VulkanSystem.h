#pragma once

#include "Macros.h"
#include "Singleton.h"
#include "Utils.h"
#include "Window.h"
#include "VulkanWindowContext.h"
#include "Subsystems/VulkanMemAllocSubsystem.h"
#include "Subsystems/VulkanSyncSubsystem.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Engine {

    /**
     * @brief Core Vulkan system.
     *
     * Owns the VkInstance, physical/logical device, queues, per-window
     * surface + swapchain contexts, and all Vulkan subsystems.
     * Initialised once from Application::Start().
     */
    class ENGINE_API VulkanSystem final : public Singleton<VulkanSystem> {
        friend class Singleton;
    public:
        // ── Window contexts ───────────────────────────────────────────────────────

        // Creates a Vulkan surface and swapchain for the given window.
        void CreateVulkanWindowContext(uint32_t windowId, const Ref<Window>& window) const;

        // Destroys the surface and swapchain for the given window.
        void DestroyVulkanWindowContext(uint32_t windowId) const;

        // Returns the Vulkan context (surface + swapchain) for the given window.
        [[nodiscard]] const VulkanWindowContext& GetWindowContext(uint32_t windowId) const;

        // ── Subsystems ────────────────────────────────────────────────────────────
        [[nodiscard]] Ref<VulkanMemAllocSubsystem> GetMemAllocSubsystem() const;
        [[nodiscard]] Ref<VulkanSyncSubsystem>     GetSyncSubsystem()     const;

        // ── Device utilities ──────────────────────────────────────────────────────
        void WaitDeviceIdle() const;

        // ── Raw Vulkan handles ────────────────────────────────────────────────────
        [[nodiscard]] VkInstance       GetVkInstance()               const;
        [[nodiscard]] VkPhysicalDevice GetVkPhysicalDevice()         const;
        [[nodiscard]] VkDevice         GetVkDevice()                 const;
        [[nodiscard]] VkQueue          GetVkGraphicsQueue()          const;
        [[nodiscard]] uint32_t         GetVkGraphicsQueueIndex()     const;
        [[nodiscard]] VkQueue          GetVkPresentationQueue()      const;
        [[nodiscard]] uint32_t         GetVkPresentationQueueIndex() const;

#ifdef ENGINE_DEBUG
        [[nodiscard]] VkDebugUtilsMessengerEXT GetDebugMessenger() const;
#endif

    private:
        VulkanSystem();
        ~VulkanSystem() override;

        class Impl;
        Scope<Impl> pImpl;
    };

} // namespace Engine