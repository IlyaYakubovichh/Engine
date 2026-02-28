#pragma once

#include "Layer.h"
#include "Macro.h"

namespace Engine {

    class ENGINE_API Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Start();
        void Run();
        void Shutdown();

        // Interface over LayerSystem
        void PushLayer(Layer* layer);
        void PushOverlay(Layer* overlay);
        void PopLayer(const Layer* layer);
        void PopOverlay(const Layer* overlay);
    };

}