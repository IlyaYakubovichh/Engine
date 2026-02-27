#include "Application.h"
#include "Systems/LogSystem/LogSystem.h"

namespace Engine {

    void Application::Start() {
        LogSystem::Start(); ENGINE_LOG_INFO("Engine", "LogSystem ON!");
    }

    void Application::Run() {
        while (false) {

        }
    }

    void Application::Shutdown() {
        ENGINE_LOG_INFO("Engine", "LogSystem OFF!"); LogSystem::Shutdown();
    }

}