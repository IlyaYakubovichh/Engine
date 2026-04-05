#pragma once
#include "Macros.h"
#include "Utils.h"
#include "Resources/Image.h"
#include "Pipelines/Pipeline.h"
#include <glm/glm.hpp>

namespace Engine {

    /**
     * @brief Abstract rendering backend interface.
     *
     * Lifecycle: Initialize → AddWindow(s) → [BeginFrame → BeginWindow →
     * draw calls → EndWindow → EndFrame] → RemoveWindow(s) → Shutdown.
     */
    class ENGINE_API RendererAPI : public NonCopyable {
    public:
        virtual ~RendererAPI() = default;

        virtual void Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void            AddWindow(uint32_t windowId)                                        = 0;
        virtual void            RemoveWindow(uint32_t windowId)                                     = 0;
        virtual void            BeginWindow(uint32_t windowId)                                      = 0;
        virtual void            EndWindow()                                                         = 0;
        virtual void            BeginFrame()                                                        = 0;
        virtual void            EndFrame()                                                          = 0;
        virtual void            BeginRenderPass()                                                   = 0;
        virtual void            EndRenderPass()                                                     = 0;
        virtual void            SetRenderTarget(Ref<Image> rt)                                      = 0;
        virtual void            Clear(glm::vec4 color)                                              = 0;
        virtual Ref<Pipeline>   CreateGraphicsPipeline(const GraphicsPipelineSettings& settings)    = 0;
        virtual Ref<Pipeline>   CreateComputePipeline(const ComputePipelineSettings& settings)      = 0;
        virtual void            BindPipeline(Ref<Pipeline> pipeline)                                = 0;
        virtual void            Dispatch(uint32_t x, uint32_t y, uint32_t z)                        = 0;

        [[nodiscard]] static Ref<RendererAPI>   Create();
        [[nodiscard]] static Ref<RendererAPI>&  GetActive();

    protected:
        RendererAPI() = default;
    };

} // namespace Engine