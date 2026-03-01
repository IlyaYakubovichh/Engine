//
// Created by ilya on 27.02.2026.
//

#include "Window.h"
#include "LogSystem/LogSystem.h"

namespace Engine {

    Window::Window(const WindowSettings& settings) : mSettings(settings) {
        ENGINE_ASSERT_MESSAGE(settings.width > 0, "Window width must be greater than 0");
        ENGINE_ASSERT_MESSAGE(settings.height > 0, "Window height must be greater than 0");

        mWindow = glfwCreateWindow(settings.width, settings.height, settings.title, nullptr, nullptr);

        if (!mWindow) {
            ENGINE_LOG_ERROR("Window", "Failed to create GLFWwindow: {}", mSettings.title);
            return;
        }

        glfwSetWindowPos(mWindow, settings.xpos, settings.ypos);
    }

    Window::~Window() {
        if (mWindow) {
            glfwDestroyWindow(mWindow);
            mWindow = nullptr;
        }
    }

    GLFWwindow* Window::GetRawGLFW() const {
        ENGINE_ASSERT_MESSAGE(mWindow != nullptr, "Attempting to access nullptr GLFWwindow!");
        return mWindow;
    }

    bool Window::IsValid() const {
        return mWindow != nullptr;
    }

    bool Window::ShouldClose() const {
        return mWindow && glfwWindowShouldClose(mWindow);
    }

    void Window::Close() const {
        if (mWindow) {
            glfwSetWindowShouldClose(mWindow, GLFW_TRUE);
        }
    }

}