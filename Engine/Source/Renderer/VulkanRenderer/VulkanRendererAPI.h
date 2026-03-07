// VulkanRendererAPI.h
#pragma once

#include "RendererAPI.h"
#include "Macro.h"

namespace Engine {

    class ENGINE_API VulkanRendererAPI final : public RendererAPI {
    public:
        VulkanRendererAPI();
        ~VulkanRendererAPI() override = default;

        void Start()    override;
        void Shutdown() override;

        void BeginFrame()      override;
        void EndFrame()        override;
        void BeginRenderPass() override;
        void EndRenderPass()   override;
        void Present()         override;
    };

} // Engine