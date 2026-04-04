#include "GraphicsLayer.h"

void GraphicsLayer::OnAttach()
{
    Engine::Renderer::Initialize();

    for (auto& wd : mWindows) {
        Engine::Renderer::AddWindow(wd.entry.id);

        auto [w, h] = wd.entry.window->GetExtent();

        wd.renderTarget = Engine::Image::Create({
            .width      = static_cast<uint32_t>(w),
            .height     = static_cast<uint32_t>(h),
            .format     = Engine::ImageFormat::RGBA8_Unorm,
            .usage      = Engine::ImageUsage::RenderTarget,
            });
    }
}

void GraphicsLayer::OnDetach()
{
    for (auto& wd : mWindows) {
        if (!wd.renderTarget) continue;

        Engine::Renderer::RemoveWindow(wd.entry.id);
        wd.renderTarget->Free();
        wd.renderTarget.reset();
    }

    Engine::Renderer::Shutdown();
}

void GraphicsLayer::OnUpdate()
{
    static constexpr glm::vec4 kColors[] = {
        { 1.0f, 0.0f, 0.0f, 1.0f },
        { 0.0f, 1.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f, 1.0f },
    };

    for (auto& wd : mWindows) {
        if (!wd.entry.window || !wd.entry.window->ShouldClose()) continue;
        if (!wd.renderTarget) continue;

        Engine::Renderer::RemoveWindow(wd.entry.id);
        wd.renderTarget->Free();
        wd.renderTarget.reset();
        wd.entry.window.reset();
    }

    Engine::Renderer::BeginFrame();

    for (size_t i = 0; i < mWindows.size(); ++i) {
        auto& wd = mWindows[i];
        if (!wd.renderTarget) continue;

        Engine::Renderer::BeginWindow(wd.entry.id);
        Engine::Renderer::SetRenderTarget(wd.renderTarget);
        Engine::Renderer::Clear(kColors[i % std::size(kColors)]);
        Engine::Renderer::EndWindow();
    }

    Engine::Renderer::EndFrame();
}