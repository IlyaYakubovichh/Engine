//
// Created by ilya on 27.02.2026.
//

#ifndef ENGINE_WINDOW_H
#define ENGINE_WINDOW_H

#include "Macro.h"
#include "Utility.h"
#include <utility>

struct GLFWwindow;

namespace Engine {

    struct ENGINE_API WindowSettings {
        int xpos{ 100 };
        int ypos{ 100 };
        int width{ 800 };
        int height{ 600 };
        const char* title{ "Engine Window" };
    };

    class ENGINE_API Window final : NonCopyable {
    public:
        explicit Window(const WindowSettings& settings);
        ~Window() override;

        [[nodiscard]] GLFWwindow* GetRawGLFW() const;
        [[nodiscard]] std::pair<int, int> GetExtent() const;
        [[nodiscard]] bool IsValid() const;
        [[nodiscard]] bool ShouldClose() const;

        void Close() const;

    private:
        GLFWwindow* mWindow{ nullptr };
        WindowSettings mSettings;
    };

}

#endif //ENGINE_WINDOW_H