#include "GraphicsLayer.h"

void GraphicsLayer::OnAttach()
{
    Engine::Renderer::Initialize();

    mRenderTarget = Engine::Image::Create({
        .width = 800,
        .height = 600,
        .format = Engine::ImageFormat::RGBA8_Unorm,
        .usage = Engine::ImageUsage::RenderTarget,
        });
}

void GraphicsLayer::OnDetach()
{
    mRenderTarget->Free();
    mRenderTarget.reset();
    Engine::Renderer::Shutdown();
}

void GraphicsLayer::OnUpdate()
{
    Engine::Renderer::BeginFrame();
    Engine::Renderer::SetRenderTarget(mRenderTarget);
    Engine::Renderer::Clear({ 0.1f, 0.5f, 0.1f, 1.0f });
    Engine::Renderer::BeginRenderPass();

    // TODO: draw calls

    Engine::Renderer::EndRenderPass();
    Engine::Renderer::EndFrame();
    Engine::Renderer::Present();
}

void GraphicsLayer::OnEvent()
{
}