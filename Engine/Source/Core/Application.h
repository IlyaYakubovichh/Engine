#pragma once

#include "Layer.h"
#include "Macro.h"
#include "Systems/WindowSystem/WindowSystem.h"

namespace Engine {

    class ENGINE_API Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Start();
        void Run();
        void Shutdown();

        // Creates a window + its VulkanWindowContext in one call
        std::pair<uint32_t, std::shared_ptr<Window>> CreateWindow(const WindowSettings& settings);

        // Interface over LayerSystem
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(Layer* layer);
        void PopOverlay(Layer* overlay);

    private:
        void DetachLayers();
    };

} // namespace Engine