//
// Created by ilya on 28.02.2026.

#include "SandBoxGraphicsLayer.h"

void SandBoxGraphicsLayer::OnAttach() {
    // Renderer initialize
    Engine::RendererSettings settings;
    settings.rendererAPI = Engine::API::Vulkan;
    Engine::Renderer::Initialize(settings);

    // Create render target
	Engine::ImageSpec imageSpec;
	imageSpec.width = 1280;
	imageSpec.height = 720;
	imageSpec.format = Engine::ImageFormat::RGBA8_UNORM;
	imageSpec.usage = Engine::ImageUsage::RenderTarget;
	mRenderTarget = Engine::Image::Create(imageSpec);
}

void SandBoxGraphicsLayer::OnDetach() {
    mRenderTarget.reset();
    Engine::Renderer::Shutdown();
}

void SandBoxGraphicsLayer::OnUpdate() {
    Engine::Renderer::BeginFrame();
    Engine::Renderer::BeginRenderPass(mRenderTarget);
    Engine::Renderer::Clear(glm::vec4{1.0f, 0.0f, 0.0f, 1.0f});
    Engine::Renderer::EndRenderPass();
    Engine::Renderer::EndFrame();
    Engine::Renderer::Present();
}

void SandBoxGraphicsLayer::OnEvent() {

}