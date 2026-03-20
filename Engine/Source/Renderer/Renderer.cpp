#include "Renderer.h"
#include "Systems/LogSystem/LogSystem.h"

namespace Engine {

    Ref<RendererAPI> Renderer::sRendererAPI;

    void Renderer::Initialize()
    {
        sRendererAPI = RendererAPI::Create();
        if (!sRendererAPI) {
            ENGINE_LOG_ERROR("Renderer", "Failed to create RendererAPI");
            return;
        }
        sRendererAPI->Initialize();
    }

    void Renderer::Shutdown()
    {
        // Renderer API
        {
            if (sRendererAPI) sRendererAPI->Shutdown();
            sRendererAPI.reset();
        }
    }

    void Renderer::BeginFrame()                         { sRendererAPI->BeginFrame();               }
    void Renderer::EndFrame()                           { sRendererAPI->EndFrame();                 }
    void Renderer::BeginRenderPass()                    { sRendererAPI->BeginRenderPass();          }
    void Renderer::EndRenderPass()                      { sRendererAPI->EndRenderPass();            }
    void Renderer::Present()                            { sRendererAPI->Present();                  }
    void Renderer::SetRenderTarget(Ref<Image> target)   { sRendererAPI->SetRenderTarget(target);    }
    void Renderer::Clear(glm::vec4 clearColor)          { sRendererAPI->Clear(clearColor);          }

} // namespace Engine