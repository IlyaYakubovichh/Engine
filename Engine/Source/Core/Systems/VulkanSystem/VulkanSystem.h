#ifndef ENGINE_VULKANSYSTEM_H
#define ENGINE_VULKANSYSTEM_H

#include "Singleton.h"
#include <vulkan/vulkan.h>
#include <memory>

namespace Engine {

    class VulkanSystem final : public Singleton<VulkanSystem> {
        friend class Singleton;

    public:
        [[nodiscard]] VkInstance       GetInstance()           const;
        [[nodiscard]] VkPhysicalDevice GetPhysicalDevice()     const;
        [[nodiscard]] VkDevice         GetDevice()             const;
        [[nodiscard]] VkSurfaceKHR     GetSurface()            const;
        [[nodiscard]] VkQueue          GetGraphicsQueue()      const;
        [[nodiscard]] uint32_t         GetGraphicsQueueIndex() const;

#ifdef ENGINE_DEBUG
        [[nodiscard]] VkDebugUtilsMessengerEXT GetDebugMessenger() const;
#endif

    private:
        VulkanSystem();
        ~VulkanSystem() override;

        // PIMPL
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace Engine

#endif // ENGINE_VULKANSYSTEM_H