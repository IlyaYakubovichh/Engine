#pragma once
#include "Macros.h"
#include "Utils.h"
#include "RendererAPI.h"
#include "Image.h"
#include <glm/glm.hpp>

namespace Engine {

    /**
     * @brief Static facade over the active RendererAPI.
     *
     * All rendering calls go through here. The active backend is
     * created in Initialize() and released in Shutdown().
     *
     * Use OpenScope(windowId) / CloseScope() to target a specific window.
     * All calls between them are routed to that window's swapchain.
     */
    class ENGINE_API Renderer final {
    public:
        Renderer() = delete;
        ~Renderer() = delete;

        static void Initialize(const RendererSettings& settings);
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();
        static void BeginRenderPass();
        static void EndRenderPass();
        static void Present();

        static void SetRenderTarget(Ref<Image> target);
        static void Clear(glm::vec4 clearColor);

    private:
        static Ref<RendererAPI> sRendererAPI;
    };

} // namespace Engine