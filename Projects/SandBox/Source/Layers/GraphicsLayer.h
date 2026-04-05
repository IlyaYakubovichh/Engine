#pragma once
#include <Engine.h>

struct WindowEntry {
    uint32_t                    id;
    Engine::Ref<Engine::Window> window;
};

class GraphicsLayer final : public Engine::Layer {
public:
    explicit GraphicsLayer(std::initializer_list<WindowEntry> windows)
        : Engine::Layer("GraphicsLayer")
    {
        mWindows.reserve(windows.size());
        for (auto& e : windows)
            mWindows.push_back({ e, nullptr });
    }

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent()  override {}

private:
    struct WindowData {
        WindowEntry                entry;
        Engine::Ref<Engine::Image> renderTarget;
    };

    std::vector<WindowData> mWindows;
    Engine::Ref<Engine::Pipeline> mComputePipeline;
    Engine::Ref<Engine::Pipeline> mGraphicsPipeline;
};