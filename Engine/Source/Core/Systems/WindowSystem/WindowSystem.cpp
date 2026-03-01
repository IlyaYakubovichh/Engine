//
// Created by ilya on 27.02.2026.
//

#include "WindowSystem/WindowSystem.h"
#include "LogSystem/LogSystem.h"
#include <GLFW/glfw3.h>

namespace Engine {

    static void GLFWErrorCallback(int error, const char* description) {
        ENGINE_LOG_ERROR("GLFW", "GLFW error: {}, description: {}", error, description);
    }

    std::pair<uint32_t, std::shared_ptr<Window>> WindowSystem::CreateWindow(const WindowSettings& settings) {
        if (settings.width == 0 || settings.height == 0) {
            ENGINE_LOG_WARN("WindowSystem", "Window width or height is 0, can't create a window!");
            return std::make_pair(0, nullptr);
        }

        if (!mInitialized) {
            ENGINE_LOG_WARN("WindowSystem", "Can't create a window, GLFW is not initialized!");
            return std::make_pair(0, nullptr);
        }

        auto window = std::make_shared<Window>(settings);
        if (!window->IsValid()) {
            ENGINE_LOG_WARN("WindowSystem", "Failed to create a window!");
        }

        uint32_t windowId = ++mCurrentWindowId;
        mWindows[windowId] = window;
        return std::make_pair(windowId, window);
    }

    void WindowSystem::DeleteWindow(const uint32_t windowId) {
        if (const auto erasedCount = mWindows.erase(windowId); erasedCount == 0) {
            ENGINE_LOG_WARN("WindowSystem", "Failed to erase a window!");
        }
    }

    bool WindowSystem::AreAllWindowsClosed() {
        bool result { true };
        for (const auto &window: mWindows | std::views::values) { // C++20 range adaptor
            result &= window->ShouldClose();
        }

        return result;
    }

    void WindowSystem::OnUpdate() {
        if (!mInitialized) return;

        glfwPollEvents();

        DeleteClosedWindows();
    }

    void WindowSystem::DeleteClosedWindows() {
        auto it = mWindows.begin();
        while (it != mWindows.end()) {
            if (it->second->ShouldClose()) {
                ENGINE_LOG_INFO("WindowSystem", "Removed window with id: {}", it->first);
                it = mWindows.erase(it);
                continue;
            }
            ++it;
        }
    }

    std::shared_ptr<Window> WindowSystem::GetWindowById(const uint32_t windowId) {
        if (const auto it = mWindows.find(windowId); it != mWindows.end()) {
            return it->second;
        }

        ENGINE_LOG_WARN("WindowSystem", "Window does not exist!");
        return nullptr;
    }

    WindowSystem::WindowSystem() {
        if (!glfwInit()) {
            ENGINE_LOG_FATAL("WindowSystem", "Failed to initialize GLFW!");
            return;
        }

        glfwSetErrorCallback(GLFWErrorCallback);
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API); // not using OpenGL API
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: make resizable

        mInitialized = true;
    }

    WindowSystem::~WindowSystem() {
        mWindows.clear();
        if (mInitialized) {
            glfwSetErrorCallback(nullptr);
            glfwTerminate();
        }

        mInitialized = false;
    }

}