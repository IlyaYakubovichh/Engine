#include "WindowSystem/WindowSystem.h"
#include "VulkanSystem/VulkanSystem.h"
#include "LogSystem/LogSystem.h"
#include <GLFW/glfw3.h>

namespace Engine {

// ---------------------------------------------------------------------------
// PIMPL
// ---------------------------------------------------------------------------
    class WindowSystem::Impl {
    public:
        Impl() {
            if (!glfwInit()) {
                ENGINE_LOG_FATAL("WindowSystem", "Failed to initialize GLFW!");
                return;
            }

            glfwSetErrorCallback(GLFWErrorCallback);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: make resizable

            mInitialized = true;
            ENGINE_LOG_DEBUG("WindowSystem", "GLFW initialized successfully");
        }

        ~Impl() {
            mWindows.clear();
            ENGINE_LOG_DEBUG("WindowSystem", "All windows destroyed");

            if (mInitialized) {
                glfwSetErrorCallback(nullptr);
                glfwTerminate();
                ENGINE_LOG_DEBUG("WindowSystem", "GLFW terminated");
            }
        }

        std::pair<uint32_t, std::shared_ptr<Window>> CreateWindow(const WindowSettings& settings) {
            if (settings.width == 0 || settings.height == 0) {
                ENGINE_LOG_WARN("WindowSystem", "Window width or height is 0, can't create a window!");
                return { 0, nullptr };
            }

            if (!mInitialized) {
                ENGINE_LOG_WARN("WindowSystem", "Can't create a window, GLFW is not initialized!");
                return { 0, nullptr };
            }

            auto window = std::make_shared<Window>(settings);
            if (!window->IsValid()) {
                ENGINE_LOG_WARN("WindowSystem", "Failed to create a window!");
                return { 0, nullptr };
            }

            const uint32_t windowId = ++mCurrentWindowId;
            mWindows[windowId]      = window;

            ENGINE_LOG_DEBUG("WindowSystem", "Window created with id: {}", windowId);
            return { windowId, window };
        }

        void DeleteWindow(const uint32_t windowId) {
            if (const auto erasedCount = mWindows.erase(windowId); erasedCount == 0) {
                ENGINE_LOG_WARN("WindowSystem", "Failed to erase window with id: {}", windowId);
                return;
            }
            ENGINE_LOG_INFO("WindowSystem", "Window with id {} deleted", windowId);
        }

        void OnUpdate() {
            if (!mInitialized) return;

            glfwPollEvents();
            DeleteClosedWindows();
        }

        [[nodiscard]] bool AreAllWindowsClosed() const {
            for (const auto& window : mWindows | std::views::values) {
                if (!window->ShouldClose()) return false;
            }
            return true;
        }

        [[nodiscard]] std::shared_ptr<Window> GetWindowById(const uint32_t windowId) const {
            if (const auto it = mWindows.find(windowId); it != mWindows.end()) {
                return it->second;
            }
            ENGINE_LOG_WARN("WindowSystem", "Window with id {} does not exist!", windowId);
            return nullptr;
        }

    private:
        void DeleteClosedWindows() {
            auto it = mWindows.begin();
            while (it != mWindows.end()) {
                if (it->second->ShouldClose()) {
                    ENGINE_LOG_INFO("WindowSystem", "Removed closed window with id: {}", it->first);
                    VulkanSystem::GetInstance()->DestroyVulkanWindowContext(it->first);
                    it = mWindows.erase(it);
                    continue;
                }
                ++it;
            }
        }

        static void GLFWErrorCallback(int error, const char* description) {
            ENGINE_LOG_ERROR("GLFW", "Error {}: {}", error, description);
        }

        std::unordered_map<uint32_t, std::shared_ptr<Window>> mWindows;
        uint32_t mCurrentWindowId { 0 };
        bool     mInitialized     { false };
    };

// ---------------------------------------------------------------------------
// WindowSystem - thin shell
// ---------------------------------------------------------------------------
    WindowSystem::WindowSystem()
        : pImpl(std::make_unique<Impl>()) {}

    WindowSystem::~WindowSystem() = default;

    std::pair<uint32_t, std::shared_ptr<Window>> WindowSystem::CreateWindow(const WindowSettings& settings) const {
        return pImpl->CreateWindow(settings);
    }

    void WindowSystem::DeleteWindow(const uint32_t windowId) const {
        pImpl->DeleteWindow(windowId);
    }

    void WindowSystem::OnUpdate() const {
        pImpl->OnUpdate();
    }

    bool WindowSystem::AreAllWindowsClosed() const {
        return pImpl->AreAllWindowsClosed();
    }

    std::shared_ptr<Window> WindowSystem::GetWindowById(const uint32_t windowId) const {
        return pImpl->GetWindowById(windowId);
    }

} // namespace Engine