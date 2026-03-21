#pragma once
#include <Engine.h>

/**
 * @brief Sample layer that clears the screen each frame.
 *
 * Owns the render target image and drives the Renderer lifecycle.
 * Tied to a specific window via windowId.
 */
class GraphicsLayer final : public Engine::Layer {
public:
    GraphicsLayer(uint32_t windowId, Engine::Ref<Engine::Window> window)
        : Layer("GraphicsLayer")
        , mWindowId(windowId)
        , mWindow(std::move(window))
    {
    }

    ~GraphicsLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent()  override;

private:
    uint32_t                    mWindowId;
    Engine::Ref<Engine::Window> mWindow;
    Engine::Ref<Engine::Image>  mRenderTarget;
};