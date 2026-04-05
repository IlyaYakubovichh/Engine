#include "VulkanComputePipeline.h"
#include "Vulkan/VulkanSystem.h"
#include "Shaders/VulkanShader.h"
#include "VulkanUtils.h"
#include "Log/LogSystem.h"

namespace Engine {

    VulkanComputePipeline::VulkanComputePipeline(const ComputePipelineSettings& settings)
        : mSettings(settings)
        , mDevice(VulkanSystem::GetInstance()->GetVkDevice())
    {
        Build();
    }

    VulkanComputePipeline::~VulkanComputePipeline()
    {
        Destroy();
    }

    void VulkanComputePipeline::Build()
    {
        const VkPipelineLayoutCreateInfo layoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        };
        VK_CHECK(vkCreatePipelineLayout(mDevice, &layoutInfo, nullptr, &mLayout));

        const auto* shader = static_cast<VulkanShader*>(mSettings.shader.get());

        const VkComputePipelineCreateInfo pipelineInfo{
            .sType  = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
            .stage  = shader->GetStageInfo(),
            .layout = mLayout,
        };

        VK_CHECK(vkCreateComputePipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline));
        ENGINE_LOG_INFO("VulkanComputePipeline", "Built");
    }

    void VulkanComputePipeline::Destroy()
    {
        if (mPipeline != VK_NULL_HANDLE) {
            vkDestroyPipeline(mDevice, mPipeline, nullptr);
            mPipeline = VK_NULL_HANDLE;
        }
        if (mLayout != VK_NULL_HANDLE) {
            vkDestroyPipelineLayout(mDevice, mLayout, nullptr);
            mLayout = VK_NULL_HANDLE;
        }
    }

} // namespace Engine