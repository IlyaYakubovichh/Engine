#ifndef ENGINE_WINDOWSYSTEM_H
#define ENGINE_WINDOWSYSTEM_H

#include "Singleton.h"
#include "Window/Window.h"
#include <memory>
#include <utility>

namespace Engine {

    constexpr uint32_t gMainWindowId = 1;

    class WindowSystem final : public Singleton<WindowSystem> {
        friend class Singleton;

    public:
        std::pair<uint32_t, std::shared_ptr<Window>> CreateWindow(const WindowSettings& settings) const;
        void DeleteWindow(uint32_t windowId) const;
        void OnUpdate() const;

        [[nodiscard]] bool AreAllWindowsClosed() const;
        [[nodiscard]] std::shared_ptr<Window> GetWindowById(uint32_t windowId) const;

    private:
        WindowSystem();
        ~WindowSystem() override;

        // PIMPL
        class Impl;
        std::unique_ptr<Impl> pImpl;
    };

} // namespace Engine

#endif // ENGINE_WINDOWSYSTEM_H