//
// Created by ilya on 28.02.2026.

#include "SandBoxGraphicsLayer.h"

void SandBoxGraphicsLayer::OnAttach() const {
    // Renderer initialize
    Engine::RendererSettings settings;
    settings.rendererAPI = Engine::API::Vulkan;
    settings.rendererType = Engine::RendererType::Renderer2D;
    Engine::Renderer::Initialize(settings);

    // Create render target
    /*
     * Engine::ImageSettings imageSettings;
     * imageSettings.width  = 1280;
     * imageSettings.height = 720;
     * imageSettings.format = Engine::ImageFormat::RGBA_8BIT_UNORM;
     * imageSettings.usage = Engine::ImageUsage::RenderTarget;
     * mRenderTarget = Engine::Renderer::CreateImage(imageSettings);
     */
}

void SandBoxGraphicsLayer::OnDetach() const {
    // mRenderTarget.reset();
    Engine::Renderer::Shutdown();
}

void SandBoxGraphicsLayer::OnUpdate() const {
    Engine::Renderer::BeginFrame();
    Engine::Renderer::BeginRenderPass(/* mRenderTarget */);
    Engine::Renderer::Clear(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    Engine::Renderer::EndRenderPass();
    Engine::Renderer::EndFrame();
    Engine::Renderer::Present();
}

void SandBoxGraphicsLayer::OnEvent() const {

}