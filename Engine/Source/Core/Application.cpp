#include "Application.h"
#include "Macro.h"
#include "Systems/LogSystem/LogSystem.h"

namespace Engine {

    Application::Application() {
        LogSystem::GetInstance();

        // Log system test
        ENGINE_LOG_TRACE("TestLevel", "Checking TRACE level");
        ENGINE_LOG_DEBUG("TestLevel", "Checking DEBUG level");
        ENGINE_LOG_INFO("TestLevel",  "Checking INFO level");
        ENGINE_LOG_WARN("TestLevel",  "Checking WARN level");
        ENGINE_LOG_ERROR("TestLevel", "Checking ERROR level");
        ENGINE_LOG_FATAL("TestLevel", "Checking FATAL level");

        // Assertions test
        int health = 100;
        int maxHealth = 100;
        ENGINE_ASSERT(health == maxHealth);
        ENGINE_ASSERT_MESSAGE(health > 0, "Player health must be negative!");
    }


    void Application::Run() {

    }

}