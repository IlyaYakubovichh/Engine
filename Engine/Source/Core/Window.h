#pragma once

#include "Macros.h"
#include "Utils.h"
#include <utility>

struct GLFWwindow;

namespace Engine {

    // Construction parameters for a platform window.
    struct ENGINE_API WindowSettings {
        int         xpos{ 100 };
        int         ypos{ 100 };
        int         width{ 800 };
        int         height{ 600 };
        const char* title{ "Engine Window" };
    };

    /**
     * @brief Thin RAII wrapper around a GLFWwindow.
     *
     * Non-copyable. Destruction automatically calls glfwDestroyWindow.
     */
    class ENGINE_API Window final : public NonCopyable {
    public:
        explicit Window(const WindowSettings& settings);
        ~Window() override;

        // Returns the underlying GLFW handle. Asserts if null.
        [[nodiscard]] GLFWwindow* GetRawGLFW() const;

        // Returns (width, height) in screen coordinates.
        [[nodiscard]] std::pair<int, int>  GetExtent()  const;

        [[nodiscard]] bool IsValid()      const;
        [[nodiscard]] bool ShouldClose()  const;

        void Close();

    private:
        GLFWwindow* mWindow{ nullptr };
        WindowSettings mSettings;
        bool mClosed{ false };
    };

} // namespace Engine