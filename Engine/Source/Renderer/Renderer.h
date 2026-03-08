#pragma once

#include "RendererAPI.h"
#include "Macro.h"
#include "Utility.h"
#include <glm/glm.hpp>

namespace Engine {

    enum class RendererType : uint8_t {
        None       = LEFT_SHIFT(0),
        Renderer2D = LEFT_SHIFT(1),
        Renderer3D = LEFT_SHIFT(2)
    };

    struct RendererSettings {
        API          rendererAPI  { API::None };
        RendererType rendererType { RendererType::None };
    };

    class ENGINE_API Renderer final {
    public:
        static void Initialize(const RendererSettings& settings);
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();
        static void BeginRenderPass();
        static void EndRenderPass();
        static void Present();

        static void Clear(glm::vec4 clearColor);

        [[nodiscard]] static const API&              GetAPI()             { return sRendererAPI->GetAPI(); }
        [[nodiscard]] static const RendererSettings& GetRendererSettings(){ return sRendererSettings; }

    private:
        Renderer()  = default;
        ~Renderer() = default;

        static Ref<RendererAPI>  sRendererAPI;
        static RendererSettings  sRendererSettings;
    };

} // Engine