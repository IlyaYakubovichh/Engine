//
// Created by ilya on 28.02.2026.
//

#ifndef ENGINE_SANDBOXGRAPHICSLAYER_H
#define ENGINE_SANDBOXGRAPHICSLAYER_H

#include <Engine.h>

class SandBoxGraphicsLayer final : public Engine::Layer {
public:
    SandBoxGraphicsLayer() : Layer("Graphics layer") {

    }

    ~SandBoxGraphicsLayer() override {

    }

    void OnAttach() const override;
    void OnDetach() const override;
    void OnUpdate() const override;
    void OnEvent() const override;

private:
    // Ref<Engine::Image> mRenderTarget;
};


#endif //ENGINE_SANDBOXGRAPHICSLAYER_H