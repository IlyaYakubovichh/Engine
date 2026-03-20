#include "Window.h"
#include "Macros.h"
#include "Systems/LogSystem/LogSystem.h"
#include <GLFW/glfw3.h>

namespace Engine {

    Window::Window(const WindowSettings& settings)
        : mSettings(settings)
    {
        ENGINE_ASSERT_MSG(settings.width > 0, "Window width must be > 0");
        ENGINE_ASSERT_MSG(settings.height > 0, "Window height must be > 0");

        mWindow = glfwCreateWindow(settings.width, settings.height, settings.title, nullptr, nullptr);

        if (!mWindow) {
            ENGINE_LOG_ERROR("Window", "Failed to create GLFWwindow: {}", mSettings.title);
            return;
        }

        glfwSetWindowPos(mWindow, settings.xpos, settings.ypos);
    }

    Window::~Window()
    {
        if (mWindow) {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }
    }

    GLFWwindow* Window::GetRawGLFW() const
    {
        ENGINE_ASSERT_MSG(mWindow != nullptr, "Accessing null GLFWwindow");
        return mWindow;
    }

    std::pair<int, int> Window::GetExtent() const
    {
        return { mSettings.width, mSettings.height };
    }

    bool Window::IsValid()     const { return mWindow != nullptr; }
    bool Window::ShouldClose() const { return mWindow && glfwWindowShouldClose(mWindow); }

    void Window::Close() const
    {
        if (mWindow) glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
    }

} // namespace Engine