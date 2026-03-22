#pragma once
#include <Engine.h>

class GraphicsLayer final : public Engine::Layer {
public:
    GraphicsLayer(uint32_t windowId, Engine::Ref<Engine::Window> window)
        : Engine::Layer("GraphicsLayer"),
        mWindowId(windowId),
        mWindow(std::move(window)) {
    }

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent()  override;

private:
    uint32_t                    mWindowId{};
    Engine::Ref<Engine::Window> mWindow;
    Engine::Ref<Engine::Image>  mRenderTarget;
    Engine::Ref<Engine::Shader> mSkyShader;   

    std::filesystem::path mWd = "../../../Projects/SandBox/Source/Assets";
};