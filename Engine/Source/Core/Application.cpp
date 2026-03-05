#include "Application.h"
#include "Systems/LogSystem/LogSystem.h"
#include "Systems/LayerSystem/LayerSystem.h"
#include "Systems/WindowSystem/WindowSystem.h"
#include "Systems/VulkanSystem/VulkanSystem.h"

namespace Engine {

    void Application::Start() {
        LogSystem::Start();         ENGINE_LOG_INFO("Engine", "LogSystem ON!");
        LayerSystem::Start();       ENGINE_LOG_INFO("Engine", "LayerSystem ON!");

        WindowSystem::Start();      ENGINE_LOG_INFO("Engine", "WindowSystem ON!");
        WindowSystem::GetInstance()->CreateWindow(WindowSettings{});

        VulkanSystem::Start();      ENGINE_LOG_INFO("Engine", "VulkanSystem ON!");
    }

    void Application::Run() {
        const auto& windowSystem = WindowSystem::GetInstance();
        const auto& layerSystem = LayerSystem::GetInstance();

        while (!windowSystem->AreAllWindowsClosed()) {
            for (const auto& layer : layerSystem->GetLayers()) {
                if (layer) {
                    // layer->OnUpdate();
                }
            }

            windowSystem->OnUpdate();
        }
    }

    void Application::Shutdown() {
        ENGINE_LOG_INFO("Engine", "VulkanSystem OFF!");  VulkanSystem::Shutdown();
        ENGINE_LOG_INFO("Engine", "WindowSystem OFF!");  WindowSystem::Shutdown();
        ENGINE_LOG_INFO("Engine", "LayerSystem OFF!");   LayerSystem::Shutdown();
        ENGINE_LOG_INFO("Engine", "LogSystem OFF!");     LogSystem::Shutdown();
    }

    void Application::PushLayer(Layer* layer) {
        if (layer) {
            LayerSystem::GetInstance()->PushLayer(layer);
        }
    }

    void Application::PushOverlay(Layer* overlay) {
        if (overlay) {
            LayerSystem::GetInstance()->PushOverlay(overlay);
        }
    }

    void Application::PopLayer(const Layer* layer) {
        if (layer) {
            LayerSystem::GetInstance()->PopLayer(layer);
        }
    }

    void Application::PopOverlay(const Layer* overlay) {
        if (overlay) {
            LayerSystem::GetInstance()->PopOverlay(overlay);
        }
    }

}