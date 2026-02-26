#pragma once

#include "Macro.h"

namespace Engine {

    class ENGINE_API Application {
    public:
        Application() = default;
        virtual ~Application() = default;

        static Application* GetInstance() {
            static Application instance;
            return &instance;
        }

        void Run();

    private:
        static Application* sInstance;
    };

}