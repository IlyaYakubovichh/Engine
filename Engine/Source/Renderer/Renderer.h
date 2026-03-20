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
     * TODO (multi-window): extend BeginFrame / Present to accept a window ID
     * so each window's swapchain can be driven independently.
     */
    class ENGINE_API Renderer final {
    public:
        Renderer() = delete;
        ~Renderer() = delete;

        static void Initialize();
        static void Shutdown();

        static void BeginFrame();
        static void EndFrame();
        static void BeginRenderPass();
        static void EndRenderPass();

        // Presents the current frame to the active window.
        // TODO (multi-window): add windowId parameter to target a specific swapchain.
        static void Present();

        // Sets the image that subsequent draw calls will render into.
        // TODO (multi-window): call once per window with its own render target.
        static void SetRenderTarget(Ref<Image> target);

        static void Clear(glm::vec4 clearColor);

    private:
        static Ref<RendererAPI> sRendererAPI;
    };

} // namespace Engine