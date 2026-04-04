#include "Window/WindowSystem.h"
#include "Log/LogSystem.h"
#include <GLFW/glfw3.h>

namespace Engine {

    class WindowSystem::Impl {
    public:
        Impl()
        {
            if (!glfwInit()) {
                ENGINE_LOG_FATAL("WindowSystem", "Failed to initialize GLFW");
                return;
            }

            glfwSetErrorCallback(OnGlfwError);
            mInitialized = true;
            ENGINE_LOG_DEBUG("WindowSystem", "GLFW initialized");
        }

        ~Impl()
        {
            mWindows.clear();
            ENGINE_LOG_DEBUG("WindowSystem", "All windows destroyed");

            if (mInitialized) {
                glfwSetErrorCallback(nullptr);
                glfwTerminate();
                ENGINE_LOG_DEBUG("WindowSystem", "GLFW terminated");
            }
        }

        std::pair<uint32_t, Ref<Window>> CreateWindow(const WindowSettings& settings)
        {
            if (!mInitialized) {
                ENGINE_LOG_WARN("WindowSystem", "CreateWindow: GLFW not initialized");
                return { 0, nullptr };
            }

            if (settings.width == 0 || settings.height == 0) {
                ENGINE_LOG_WARN("WindowSystem", "CreateWindow: invalid dimensions {}x{}",
                    settings.width, settings.height);
                return { 0, nullptr };
            }

            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
            glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);

            auto window = std::make_shared<Window>(settings);
            if (!window->IsValid()) {
                ENGINE_LOG_WARN("WindowSystem", "CreateWindow: GLFWwindow creation failed");
                return { 0, nullptr };
            }

            const uint32_t id = mNextId++;
            mWindows[id] = window;

            ENGINE_LOG_DEBUG("WindowSystem", "Window {} created ({}x{})",
                id, settings.width, settings.height);
            return { id, window };
        }

        void DeleteWindow(uint32_t id)
        {
            if (mWindows.erase(id) == 0) {
                ENGINE_LOG_WARN("WindowSystem", "DeleteWindow: id {} not found", id);
                return;
            }
            ENGINE_LOG_INFO("WindowSystem", "Window {} deleted", id);
        }

        void OnUpdate()
        {
            if (!mInitialized) return;
            glfwPollEvents();
            PurgeClosedWindows();
        }

        [[nodiscard]] bool AreAllWindowsClosed() const
        {
            for (const auto& window : mWindows | std::views::values) {
                if (!window->ShouldClose()) return false;
            }
            return true;
        }

        [[nodiscard]] Ref<Window> GetWindowById(uint32_t id) const
        {
            const auto it = mWindows.find(id);
            if (it != mWindows.end()) return it->second;

            ENGINE_LOG_WARN("WindowSystem", "GetWindowById: id {} not found", id);
            return nullptr;
        }

    private:

        void PurgeClosedWindows()
        {
            for (auto it = mWindows.begin(); it != mWindows.end(); ) {
                if (!it->second->ShouldClose()) {
                    ++it;
                    continue;
                }

                ENGINE_LOG_INFO("WindowSystem", "Window {} closed, purging", it->first);
                it->second->Close();
                it = mWindows.erase(it);
            }
        }

        static void OnGlfwError(int code, const char* description)
        {
            ENGINE_LOG_ERROR("GLFW", "Error {}: {}", code, description);
        }

        std::unordered_map<uint32_t, Ref<Window>> mWindows;
        uint32_t mNextId{ 0 };
        bool     mInitialized{ false };
    };

    WindowSystem::WindowSystem() : pImpl(std::make_unique<Impl>()) {}
    WindowSystem::~WindowSystem() = default;

    std::pair<uint32_t, Ref<Window>> WindowSystem::CreateWindow(const WindowSettings& settings) const
    {
        return pImpl->CreateWindow(settings);
    }

    void        WindowSystem::DeleteWindow(uint32_t id)  const { pImpl->DeleteWindow(id);               }
    void        WindowSystem::OnUpdate()                 const { pImpl->OnUpdate();                     }
    bool        WindowSystem::AreAllWindowsClosed()      const { return pImpl->AreAllWindowsClosed();   }
    Ref<Window> WindowSystem::GetWindowById(uint32_t id) const { return pImpl->GetWindowById(id);       }

} // namespace Engine