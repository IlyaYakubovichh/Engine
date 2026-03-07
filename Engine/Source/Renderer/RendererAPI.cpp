#include "RendererAPI.h"
#include "VulkanRenderer/VulkanRendererAPI.h"
#include "LogSystem/LogSystem.h"

namespace Engine {

    Ref<RendererAPI> RendererAPI::Create(const API api) {
        switch (api) {
            case API::Vulkan: return std::make_shared<VulkanRendererAPI>();
            case API::None:
            default:
                ENGINE_LOG_ERROR("RendererAPI", "RendererAPI::Create — unknown or None API requested");
                return nullptr;
        }
    }

} // Engine