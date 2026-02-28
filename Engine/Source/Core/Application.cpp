#include "Application.h"
#include "Systems/LogSystem/LogSystem.h"
#include "Systems/LayerSystem/LayerSystem.h"

namespace Engine {

    void Application::Start() {
        LogSystem::Start();     ENGINE_LOG_INFO("Engine", "LogSystem ON!");
        LayerSystem::Start();   ENGINE_LOG_INFO("Engine", "LayerSystem ON!");
    }

    void Application::Run() {
        while (true) {
            for (auto layers = LayerSystem::GetInstance()->GetLayers(); const auto layer : layers) {
                layer->OnUpdate();
            }
        }
    }

    void Application::Shutdown() {
        ENGINE_LOG_INFO("Engine", "LayerSystem OFF!");  LayerSystem::Shutdown();
        ENGINE_LOG_INFO("Engine", "LogSystem OFF!");    LogSystem::Shutdown();
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