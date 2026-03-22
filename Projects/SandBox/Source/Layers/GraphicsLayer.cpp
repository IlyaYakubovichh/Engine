#include "GraphicsLayer.h"
#include <filesystem>
#include <iostream>

void GraphicsLayer::OnAttach()
{
    // Renderer
    Engine::Renderer::Initialize({ .windowId = mWindowId });

    // Render target
    auto [width, height] = mWindow->GetExtent();
    mRenderTarget = Engine::Image::Create({
        .width    = static_cast<uint32_t>(width),
        .height   = static_cast<uint32_t>(height),
        .format   = Engine::ImageFormat::RGBA8_Unorm,
        .usage    = Engine::ImageUsage::RenderTarget,
        });
    
    // Shades
    std::filesystem::path skyShaderPath = mWd / "Shaders" / "sky.comp";
    mSkyShader = Engine::ShaderLibrary::GetInstance()->Load("sky", skyShaderPath, Engine::ShaderStage::Compute);
}

void GraphicsLayer::OnDetach()
{
    Engine::ShaderLibrary::GetInstance()->Clear();

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