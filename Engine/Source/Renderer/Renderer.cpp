#include "Renderer.h"
#include "LogSystem/LogSystem.h"

namespace Engine {

    Ref<RendererAPI> Renderer::sRendererAPI;
    RendererSettings Renderer::sRendererSettings;

    void Renderer::Initialize(const RendererSettings& settings) {
        sRendererSettings = settings;

        // RendererAPI
        sRendererAPI = RendererAPI::Create(settings.rendererAPI);
        if (!sRendererAPI) {
            ENGINE_LOG_ERROR("Renderer", "Renderer::Start — failed to create RendererAPI");
            return;
        }

        // Start
        sRendererAPI->Initialize();
    }

    void Renderer::Shutdown() {
        if (sRendererAPI) {
            sRendererAPI->Shutdown();
        }
        sRendererAPI.reset();
    }

    void Renderer::BeginFrame()      { sRendererAPI->BeginFrame();      }
    void Renderer::EndFrame()        { sRendererAPI->EndFrame();        }
    void Renderer::BeginRenderPass() { sRendererAPI->BeginRenderPass(); }
    void Renderer::EndRenderPass()   { sRendererAPI->EndRenderPass();   }
    void Renderer::Present()         { sRendererAPI->Present();         }

    void Renderer::Clear(glm::vec4 clearColor) { sRendererAPI->Clear(clearColor); }

} // Engine