#pragma once

#include "RendererAPI.h"
#include "VulkanRendererData.h"
#include "Macro.h"

namespace Engine {

    class ENGINE_API VulkanRendererAPI final : public RendererAPI {
    public:
        VulkanRendererAPI();
        ~VulkanRendererAPI() override = default;

        void Initialize() override;
        void Shutdown()   override;

        void BeginFrame()      override;
        void EndFrame()        override;
        void BeginRenderPass() override;
        void EndRenderPass()   override;
        void Present()         override;

    private:
        VulkanRendererData mData;
    };

} // Engine