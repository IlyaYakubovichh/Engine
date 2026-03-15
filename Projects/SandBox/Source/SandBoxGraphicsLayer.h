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

	void OnAttach() override;
	void OnDetach() override;
    void OnUpdate() override;
    void OnEvent()  override;

private:
    Engine::Ref<Engine::Image> mRenderTarget;
};


#endif //ENGINE_SANDBOXGRAPHICSLAYER_H