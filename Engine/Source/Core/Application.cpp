#include "Application.h"
#include "Log/LogSystem.h"
#include "File/FileSystem.h"
#include "Layer/LayerSystem.h"
#include "Window/WindowSystem.h"
#include "Vulkan/VulkanSystem.h"
#include "Shaders/ShaderLibrary.h"
#include "Pipelines/PipelineLibrary.h"

namespace Engine {

    template<typename TSystem>
    static void startSystem(const char* name)
    {
        TSystem::Start();
        ENGINE_LOG_INFO("Engine", "{} ON", name);
    }

    template<typename TSystem>
    static void shutdownSystem(const char* name)
    {
        ENGINE_LOG_INFO("Engine", "{} OFF", name);
        TSystem::Shutdown();
    }

    static void updateLayers(LayerSystem* layerSystem)
    {
        for (const auto& layer : layerSystem->GetLayers()) {
            if (layer) layer->OnUpdate();
        }
    }

    void Application::Start()
    {
        startSystem<LogSystem>("LogSystem");
        startSystem<LayerSystem>("LayerSystem");
        startSystem<WindowSystem>("WindowSystem");
        startSystem<VulkanSystem>("VulkanSystem");
        startSystem<FileSystem>("FileSystem");
        startSystem<ShaderLibrary>("ShaderLibrary");
        startSystem<PipelineLibrary>("PipelineLibrary");
    }

    void Application::Run()
    {
        auto* windowSystem = WindowSystem::GetInstance();
        auto* layerSystem = LayerSystem::GetInstance();

        while (!windowSystem->AreAllWindowsClosed()) {
            updateLayers(layerSystem);
            windowSystem->OnUpdate();
        }
    }

    void Application::Shutdown()
    {
        DetachLayers();
        shutdownSystem<PipelineLibrary>("PipelineLibrary");
        shutdownSystem<ShaderLibrary>("ShaderLibrary");
        shutdownSystem<FileSystem>("FileSystem");
        shutdownSystem<VulkanSystem>("VulkanSystem");
        shutdownSystem<WindowSystem>("WindowSystem");
        shutdownSystem<LayerSystem>("LayerSystem");
        shutdownSystem<LogSystem>("LogSystem");
    }

    std::pair<uint32_t, Ref<Window>> Application::CreateWindow(const WindowSettings& settings)
    {
        auto [id, window] = WindowSystem::GetInstance()->CreateWindow(settings);

        if (window) {
            VulkanSystem::GetInstance()->CreateSurface(id, window);
        }

        return { id, window };
    }

    void Application::PushLayer(Layer* layer)
    {
        if (!layer) return;
        layer->OnAttach();
        LayerSystem::GetInstance()->PushLayer(layer);
    }

    void Application::PushOverlay(Layer* overlay)
    {
        if (!overlay) return;
        overlay->OnAttach();
        LayerSystem::GetInstance()->PushOverlay(overlay);
    }

    void Application::PopLayer(Layer* layer)
    {
        if (!layer) return;
        layer->OnDetach();
        LayerSystem::GetInstance()->PopLayer(layer);
    }

    void Application::PopOverlay(Layer* overlay)
    {
        if (!overlay) return;
        overlay->OnDetach();
        LayerSystem::GetInstance()->PopOverlay(overlay);
    }

    void Application::DetachLayers()
    {
        for (const auto& layer : LayerSystem::GetInstance()->GetLayers()) {
            if (layer) layer->OnDetach();
        }
    }

} // namespace Engine