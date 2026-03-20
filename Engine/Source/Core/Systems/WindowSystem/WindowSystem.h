#pragma once

#include "Utils.h"
#include "Macros.h"
#include "Singleton.h"
#include "Utils.h"
#include "Window.h"
#include <utility>

namespace Engine {

    // Globally accessible ID of the primary window.
    inline constexpr uint32_t gMainWindowId = 0;

    /**
     * @brief Manages GLFW lifecycle and the collection of active windows.
     *
     * Windows are identified by a monotonically increasing uint32_t ID.
     * Closed windows are automatically cleaned up each OnUpdate() tick.
     */
    class ENGINE_API WindowSystem final : public Singleton<WindowSystem> {
        friend class Singleton;
    public:
        // Creates a GLFW window and returns its ID + shared handle.
        [[nodiscard]] std::pair<uint32_t, Ref<Window>> CreateWindow(const WindowSettings& settings) const;

        // Explicitly destroys a window by ID.
        void DeleteWindow(uint32_t windowId) const;

        // Polls GLFW events and removes any windows that requested close.
        void OnUpdate() const;

        // Returns true when every tracked window has requested close.
        [[nodiscard]] bool AreAllWindowsClosed() const;

        // Returns the window handle for the given ID, or nullptr if not found.
        [[nodiscard]] Ref<Window> GetWindowById(uint32_t windowId) const;

    private:
        WindowSystem();
        ~WindowSystem() override;

        class Impl;
        Scope<Impl> pImpl;
    };

} // namespace Engine