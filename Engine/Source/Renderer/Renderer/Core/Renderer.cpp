#include "Renderer.h"
#include "Log/LogSystem.h"

namespace Engine {

    Ref<RendererAPI> Renderer::sRendererAPI;

    void Renderer::Initialize()
    {
        sRendererAPI = RendererAPI::Create();
        ENGINE_ASSERT_MSG(sRendererAPI, "Renderer: failed to create RendererAPI");
        sRendererAPI->Initialize();
    }

    void Renderer::Shutdown()
    {
        if (sRendererAPI) {
            sRendererAPI->Shutdown();
            sRendererAPI.reset();
        }
    }

    void Renderer::AddWindow(uint32_t windowId)                                                 { sRendererAPI->AddWindow(windowId);                        }
    void Renderer::RemoveWindow(uint32_t windowId)                                              { sRendererAPI->RemoveWindow(windowId);                     }
    void Renderer::BeginFrame()                                                                 { sRendererAPI->BeginFrame();                               }
    void Renderer::EndFrame()                                                                   { sRendererAPI->EndFrame();                                 }
    void Renderer::BeginWindow(uint32_t windowId)                                               { sRendererAPI->BeginWindow(windowId);                      }
    void Renderer::EndWindow()                                                                  { sRendererAPI->EndWindow();                                }
    void Renderer::BeginRenderPass()                                                            { sRendererAPI->BeginRenderPass();                          }
    void Renderer::EndRenderPass()                                                              { sRendererAPI->EndRenderPass();                            }
    void Renderer::SetRenderTarget(Ref<Image> rt)                                               { sRendererAPI->SetRenderTarget(rt);                        }
    void Renderer::Clear(glm::vec4 color)                                                       { sRendererAPI->Clear(color);                               }
    Ref<Pipeline> Renderer::CreateGraphicsPipeline(const GraphicsPipelineSettings& settings)    { return sRendererAPI->CreateGraphicsPipeline(settings);    }
    Ref<Pipeline> Renderer::CreateComputePipeline(const ComputePipelineSettings& settings)      { return sRendererAPI->CreateComputePipeline(settings);     }
    void Renderer::BindPipeline(Ref<Pipeline> pipeline)                                         { sRendererAPI->BindPipeline(std::move(pipeline));          }
    void Renderer::Dispatch(uint32_t x, uint32_t y, uint32_t z)                                 { sRendererAPI->Dispatch(x, y, z);                          }

} // namespace Engine