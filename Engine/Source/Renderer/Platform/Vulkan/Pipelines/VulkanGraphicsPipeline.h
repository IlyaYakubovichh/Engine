#pragma once
#include "Pipelines/GraphicsPipeline.h"
#include <vulkan/vulkan.h>

namespace Engine {

    class ENGINE_API VulkanGraphicsPipeline final : public GraphicsPipeline {
    public:
        explicit VulkanGraphicsPipeline(const GraphicsPipelineSettings& settings);
        ~VulkanGraphicsPipeline() override;

        [[nodiscard]] bool                            IsValid()             const override  { return mPipeline != VK_NULL_HANDLE;   }
        [[nodiscard]] const GraphicsPipelineSettings& GetSettings()         const override  { return mSettings;                     }
        [[nodiscard]] VkPipeline                      GetVkPipeline()       const           { return mPipeline;                     }
        [[nodiscard]] VkPipelineLayout                GetVkPipelineLayout() const           { return mLayout;                       }

    private:
        void Build();
        void Destroy();

        GraphicsPipelineSettings mSettings;
        VkPipeline               mPipeline  { VK_NULL_HANDLE };
        VkPipelineLayout         mLayout    { VK_NULL_HANDLE };
        VkDevice                 mDevice    { VK_NULL_HANDLE };
    };

} // namespace Engine