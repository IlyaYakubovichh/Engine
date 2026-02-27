#pragma once

#include "Macro.h"

namespace Engine {

    class ENGINE_API Application {
    public:
        Application();
        virtual ~Application() = default;

        void Run();
    };

}