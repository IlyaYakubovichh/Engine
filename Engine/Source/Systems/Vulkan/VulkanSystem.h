#pragma once
#include "Macros.h"
#include "Singleton.h"
#include "Utils.h"
#include "Window.h"
#include "Vulkan/VulkanMemAllocSubsystem.h"
#include "Vulkan/VulkanSyncSubsystem.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Engine {

    class ENGINE_API VulkanSystem final : public Singleton<VulkanSystem> {
        friend class Singleton;
    public:
        void CreateSurface(uint32_t windowId, const Ref<Window>& window) const;
        void DestroySurface(uint32_t windowId) const;
        void WaitIdle() const;

        [[nodiscard]] VkSurfaceKHR                  GetSurface(uint32_t windowId)   const;
        [[nodiscard]] Ref<VulkanMemAllocSubsystem>  GetMemAllocSubsystem()           const;
        [[nodiscard]] Ref<VulkanSyncSubsystem>      GetSyncSubsystem()               const;
        [[nodiscard]] VkInstance                    GetVkInstance()                  const;
        [[nodiscard]] VkPhysicalDevice              GetVkPhysicalDevice()            const;
        [[nodiscard]] VkDevice                      GetVkDevice()                    const;
        [[nodiscard]] VkQueue                       GetVkGraphicsQueue()             const;
        [[nodiscard]] uint32_t                      GetVkGraphicsQueueIndex()        const;
        [[nodiscard]] VkQueue                       GetVkPresentQueue()              const;
        [[nodiscard]] uint32_t                      GetVkPresentQueueIndex()         const;
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