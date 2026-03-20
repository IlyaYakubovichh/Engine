#include "WindowSystem.h"
#include "Systems/VulkanSystem/VulkanSystem.h"
#include "Systems/LogSystem/LogSystem.h"
#include <GLFW/glfw3.h>
#include <ranges>
#include <unordered_map>

namespace Engine {

    // ─── PIMPL ───────────────────────────────────────────────────────────────────

    class WindowSystem::Impl {
    public:
        Impl()
        {
            if (!glfwInit()) {
                ENGINE_LOG_FATAL("WindowSystem", "Failed to initialize GLFW");
                return;
            }

            glfwSetErrorCallback(OnGlfwError);
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE); // TODO: support resize

            mInitialized = true;
            ENGINE_LOG_DEBUG("WindowSystem", "GLFW initialized");
        }

        ~Impl()
        {
            // Windows must be destroyed before glfwTerminate.
            mWindows.clear();
            ENGINE_LOG_DEBUG("WindowSystem", "All windows destroyed");

            if (mInitialized) {
                glfwSetErrorCallback(nullptr);
                glfwTerminate();
                ENGINE_LOG_DEBUG("WindowSystem", "GLFW terminated");
            }
        }

        // ── Window management ────────────────────────────────────────────────────

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

            auto window = std::make_shared<Window>(settings);
            if (!window->IsValid()) {
                ENGINE_LOG_WARN("WindowSystem", "CreateWindow: GLFWwindow creation failed");
                return { 0, nullptr };
            }

            const uint32_t id = mNextId++;
            mWindows[id] = window;

            ENGINE_LOG_DEBUG("WindowSystem", "Window {} created ({}x{})", id, settings.width, settings.height);
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

        // ── Per-frame tick ───────────────────────────────────────────────────────

        void OnUpdate()
        {
            if (!mInitialized) return;
            glfwPollEvents();
            PurgeClosedWindows();
        }

        // ── Queries ──────────────────────────────────────────────────────────────

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
        // ── Helpers ──────────────────────────────────────────────────────────────

        // Destroys Vulkan context and removes every window that flagged close.
        void PurgeClosedWindows()
        {
            auto* vulkan = VulkanSystem::GetInstance();

            for (auto it = mWindows.begin(); it != mWindows.end(); ) {
                if (!it->second->ShouldClose()) {
                    ++it;
                    continue;
                }

                ENGINE_LOG_INFO("WindowSystem", "Window {} closed, purging", it->first);
                vulkan->WaitDeviceIdle();
                vulkan->DestroyVulkanWindowContext(it->first);
                it = mWindows.erase(it);
            }
        }

        static void OnGlfwError(int code, const char* description)
        {
            ENGINE_LOG_ERROR("GLFW", "Error {}: {}", code, description);
        }

        // ── State ─────────────────────────────────────────────────────────────────
        std::unordered_map<uint32_t, Ref<Window>> mWindows;
        uint32_t mNextId{ 0 };
        bool     mInitialized{ false };
    };

    // ─── WindowSystem shell ───────────────────────────────────────────────────────

    WindowSystem::WindowSystem() : pImpl(std::make_unique<Impl>()) {}
    WindowSystem::~WindowSystem() = default;

    std::pair<uint32_t, Ref<Window>> WindowSystem::CreateWindow(const WindowSettings& settings) const
    {
        return pImpl->CreateWindow(settings);
    }

    void        WindowSystem::DeleteWindow(uint32_t id)     const { pImpl->DeleteWindow(id);                }
    void        WindowSystem::OnUpdate()                    const { pImpl->OnUpdate();                      }
    bool        WindowSystem::AreAllWindowsClosed()         const { return pImpl->AreAllWindowsClosed();    }
    Ref<Window> WindowSystem::GetWindowById(uint32_t id)    const { return pImpl->GetWindowById(id);        }

} // namespace Engine