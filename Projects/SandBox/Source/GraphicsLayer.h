#pragma once
#include <Engine.h>

/**
 * @brief Sample layer that clears the screen each frame.
 *
 * Owns the render target image and drives the Renderer lifecycle.
 */
class GraphicsLayer final : public Engine::Layer {
public:
    GraphicsLayer() : Layer("GraphicsLayer") {}
    ~GraphicsLayer() override = default;

    void OnAttach() override;
    void OnDetach() override;
    void OnUpdate() override;
    void OnEvent()  override;

private:
    Engine::Ref<Engine::Image> mRenderTarget;
};