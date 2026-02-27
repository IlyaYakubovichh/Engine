#pragma once

#include "Macro.h"

namespace Engine {

    class ENGINE_API Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        void Start();
        void Run();
        void Shutdown();
    };

}