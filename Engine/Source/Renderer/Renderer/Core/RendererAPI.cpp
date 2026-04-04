#include "RendererAPI.h"
#include "Core/VulkanRendererAPI.h"

namespace Engine {
    Ref<RendererAPI> RendererAPI::Create()
    {
        return std::make_shared<VulkanRendererAPI>();
    }
} // namespace Engine