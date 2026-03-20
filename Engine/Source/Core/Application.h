#pragma once
#include "Macros.h"
#include "Utils.h"
#include "Layer.h"
#include "Window.h"

namespace Engine {

    /**
     * @brief Central engine lifecycle manager.
     *
     * Owns startup, main loop, and shutdown of all engine systems.
     * Use PushLayer / PushOverlay to register rendering and logic layers.
     */
    class ENGINE_API Application : public NonCopyable {
    public:
        // Initialises all engine systems and creates the main window.
        void Start();

        // Runs the main loop until all windows are closed.
        void Run();

        // Shuts down all engine systems in reverse order.
        void Shutdown();

        // Creates an additional window and registers it with Vulkan.
        std::pair<uint32_t, Ref<Window>> CreateWindow(const WindowSettings& settings);

        // Attaches a layer and pushes it onto the layer stack.
        void PushLayer(Layer* layer);

        // Attaches an overlay and pushes it on top of all layers.
        void PushOverlay(Layer* overlay);

        // Detaches a layer and removes it from the stack.
        void PopLayer(Layer* layer);

        // Detaches an overlay and removes it from the stack.
        void PopOverlay(Layer* overlay);

    private:
        // Calls OnDetach() on every active layer before shutdown.
        void DetachLayers();
    };

} // namespace Engine