//
// Created by ilya on 27.02.2026.
//

#ifndef ENGINE_WINDOWSYSTEM_H
#define ENGINE_WINDOWSYSTEM_H

#include "Singleton.h"
#include "Window/Window.h"
#include <unordered_map>
#include <memory>

/*
 * 1) Create a window and return std::pair(id, window);
 * 2) Get window by id
 * 3) Delete window by id
 * 4) AreAllWindowsClosed - if yes - shutdown application
*/

namespace Engine {

    class Window;

    class WindowSystem final : public Singleton<WindowSystem> {
        friend class Singleton;
    public:
        std::pair<uint32_t, std::shared_ptr<Window>> CreateWindow(const WindowSettings& settings);
        void DeleteWindow(uint32_t windowId);
        bool AreAllWindowsClosed();
        void OnUpdate();
        void DeleteClosedWindows();

        [[nodiscard]] std::shared_ptr<Window> GetWindowById(uint32_t windowId);

    private:
        WindowSystem();
        ~WindowSystem() override;

        std::unordered_map<std::uint32_t, std::shared_ptr<Window>> mWindows;
        uint32_t mCurrentWindowId { 0 };
        bool mInitialized { false };
    };

}

#endif //ENGINE_WINDOWSYSTEM_H