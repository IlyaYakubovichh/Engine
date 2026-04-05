#pragma once
#include "Macros.h"
#include "Utils.h"
#include "RendererAPI.h"
#include "Resources/Image.h"
#include <glm/glm.hpp>

namespace Engine {

    /**
     * @brief Static facade over the active RendererAPI.
     *
     * Usage per frame:
     * @code
     *   Renderer::BeginFrame();
     *     Renderer::BeginWindow(id);
     *       Renderer::SetRenderTarget(rt);
     *       Renderer::Clear({0,0,0,1});
     *     Renderer::EndWindow();
     *   Renderer::EndFrame();
     * @endcode
     */
    class ENGINE_API Renderer final {
    public:
        Renderer() = delete;
        ~Renderer() = delete;

        static void Initialize();
        static void Shutdown();

        static void             AddWindow(uint32_t windowId);
        static void             RemoveWindow(uint32_t windowId);
        static void             BeginWindow(uint32_t windowId);
        static void             EndWindow();
        static void             BeginFrame();
        static void             EndFrame();
        static void             BeginRenderPass();
        static void             EndRenderPass();
        static void             SetRenderTarget(Ref<Image> target);
        static void             Clear(glm::vec4 clearColor);
        static Ref<Pipeline>    CreateGraphicsPipeline(const GraphicsPipelineSettings& settings);
        static Ref<Pipeline>    CreateComputePipeline(const ComputePipelineSettings& settings);
        static void             BindPipeline(Ref<Pipeline> pipeline);
        static void             Dispatch(uint32_t x, uint32_t y, uint32_t z);

        [[nodiscard]] static Ref<RendererAPI>& GetAPI() { return sRendererAPI; }

    private:
        static Ref<RendererAPI> sRendererAPI;
    };

} // namespace Engine