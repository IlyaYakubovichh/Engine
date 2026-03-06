#ifndef ENGINE_VULKANSYSTEM_H
#define ENGINE_VULKANSYSTEM_H

#include "Macro.h"
#include "Singleton.h"
#include "Window.h"
#include "VulkanSystem/VulkanWindowContext.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Engine {

    class ENGINE_API VulkanSystem final : public Singleton<VulkanSystem> {
        friend class Singleton;

    public:
        void CreateVulkanWindowContext(uint32_t windowId, const std::shared_ptr<Window>& window) const;
        void DestroyVulkanWindowContext(uint32_t windowId) const;

        [[nodiscard]] const VulkanWindowContext& GetWindowContext(uint32_t windowId) const;

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
        std::unique_ptr<Impl> pImpl;
    };

} // namespace Engine

#endif // ENGINE_VULKANSYSTEM_H