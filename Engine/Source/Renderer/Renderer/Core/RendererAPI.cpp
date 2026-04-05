#include "RendererAPI.h"
#include "Renderer.h"
#include "Core/VulkanRendererAPI.h"

namespace Engine {

    Ref<RendererAPI> RendererAPI::Create()
    {
        return std::make_shared<VulkanRendererAPI>();
    }

    Ref<RendererAPI>& RendererAPI::GetActive()
    {
        return Renderer::GetAPI();
    }

} // namespace Engine