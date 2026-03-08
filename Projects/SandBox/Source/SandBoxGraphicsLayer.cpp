//
// Created by ilya on 28.02.2026.

#include "SandBoxGraphicsLayer.h"

void SandBoxGraphicsLayer::OnAttach() const {
    Engine::RendererSettings settings;
    settings.rendererAPI = Engine::API::Vulkan;
    settings.rendererType = Engine::RendererType::Renderer2D;

    Engine::Renderer::Initialize(settings);
}

void SandBoxGraphicsLayer::OnDetach() const {
    Engine::Renderer::Shutdown();
}

void SandBoxGraphicsLayer::OnUpdate() const {
    Engine::Renderer::BeginFrame();
    Engine::Renderer::BeginRenderPass();
    Engine::Renderer::Clear(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    Engine::Renderer::EndRenderPass();
    Engine::Renderer::EndFrame();
    Engine::Renderer::Present();
}

void SandBoxGraphicsLayer::OnEvent() const {

}