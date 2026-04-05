#pragma once
#include "Pipelines/ComputePipeline.h"
#include <vulkan/vulkan.h>

namespace Engine {

    class ENGINE_API VulkanComputePipeline final : public ComputePipeline {
    public:
        explicit VulkanComputePipeline(const ComputePipelineSettings& settings);
        ~VulkanComputePipeline() override;

        [[nodiscard]] bool                           IsValid()             const override   { return mPipeline != VK_NULL_HANDLE;   }
        [[nodiscard]] const ComputePipelineSettings& GetSettings()         const override   { return mSettings;                     }
        [[nodiscard]] VkPipeline                     GetVkPipeline()       const            { return mPipeline;                     }
        [[nodiscard]] VkPipelineLayout               GetVkPipelineLayout() const            { return mLayout;                       }

    private:
        void Build();
        void Destroy();

        ComputePipelineSettings mSettings;
        VkPipeline              mPipeline   { VK_NULL_HANDLE };
        VkPipelineLayout        mLayout     { VK_NULL_HANDLE };
        VkDevice                mDevice     { VK_NULL_HANDLE };
    };

} // namespace Engine