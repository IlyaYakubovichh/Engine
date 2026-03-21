#include "GraphicsLayer.h"

void GraphicsLayer::OnAttach()
{
    Engine::Renderer::Initialize({ .windowId = mWindowId });

    auto [width, height] = mWindow->GetExtent();

    mRenderTarget = Engine::Image::Create({
        .width = static_cast<uint32_t>(width),
        .height = static_cast<uint32_t>(height),
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
    Engine::Renderer::Clear({ 0.15f, 0.15f, 0.15f, 1.0f });
    Engine::Renderer::BeginRenderPass();

    // TODO: draw calls

    Engine::Renderer::EndRenderPass();
    Engine::Renderer::EndFrame();
    Engine::Renderer::Present();
}

void GraphicsLayer::OnEvent()
{
}