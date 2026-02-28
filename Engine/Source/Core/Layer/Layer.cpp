//
// Created by ilya on 28.02.2026.
//

#include "Layer.h"
#include "Systems/LogSystem/LogSystem.h"

namespace Engine {

    Layer::Layer(std::string  name)
        : mName(std::move(name)) {

    }

    Layer::~Layer() = default;

    void Layer::OnUpdate() const {
        ENGINE_LOG_INFO("Layer {}", "OnUpdate called!", mName);
    }

    void Layer::OnEvent() const {

    }

}