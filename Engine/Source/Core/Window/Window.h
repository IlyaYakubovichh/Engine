//
// Created by ilya on 27.02.2026.
//

#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "Utility.h"
#include <GLFW/glfw3.h>

namespace Engine {

    struct WindowSettings {
        int xpos{ 100 };
        int ypos{ 100 };
        int width{ 800 };
        int height{ 600 };
        const char* title{ "Engine Window" };
    };

    class Window final : NonCopyable {
    public:
        explicit Window(const WindowSettings& settings);
        ~Window() override;

        [[nodiscard]] GLFWwindow* GetRawGLFW() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] bool ShouldClose() const;

        void Close() const;

    private:
        GLFWwindow* mWindow{ nullptr };
        WindowSettings mSettings;
    };

}

#endif //ENGINE_WINDOW_H