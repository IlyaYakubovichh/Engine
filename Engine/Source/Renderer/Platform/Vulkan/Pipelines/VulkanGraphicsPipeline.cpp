#include "VulkanGraphicsPipeline.h"
#include "Vulkan/VulkanSystem.h"
#include "Shaders/VulkanShader.h"
#include "VulkanUtils.h"
#include "Log/LogSystem.h"

namespace Engine {

    VulkanGraphicsPipeline::VulkanGraphicsPipeline(const GraphicsPipelineSettings& settings)
        : mSettings(settings)
        , mDevice(VulkanSystem::GetInstance()->GetVkDevice())
    {
        Build();
    }

    VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
    {
        Destroy();
    }

    void VulkanGraphicsPipeline::Build()
    {
        // ── Layout (пустой — descriptor sets придут позже) ────────────────────────
        const VkPipelineLayoutCreateInfo layoutInfo{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
            .setLayoutCount = 0,
            .pushConstantRangeCount = 0,
        };
        VK_CHECK(vkCreatePipelineLayout(mDevice, &layoutInfo, nullptr, &mLayout));

        // ── Shader stages ─────────────────────────────────────────────────────────
        const auto* vert = static_cast<VulkanShader*>(mSettings.vertexShader.get());
        const auto* frag = static_cast<VulkanShader*>(mSettings.fragmentShader.get());

        const VkPipelineShaderStageCreateInfo stages[] = {
            vert->GetStageInfo(),
            frag->GetStageInfo(),
        };

        // ── Vertex input (пустой — bindless/buffer device address позже) ─────────
        const VkPipelineVertexInputStateCreateInfo vertexInput{
            .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        };

        // ── Input assembly ────────────────────────────────────────────────────────
        auto toVkTopology = [](PrimitiveTopology t) -> VkPrimitiveTopology {
            switch (t) {
            case PrimitiveTopology::TriangleList:  return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            case PrimitiveTopology::TriangleStrip: return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            case PrimitiveTopology::LineList:      return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            case PrimitiveTopology::PointList:     return VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            }
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            };

        const VkPipelineInputAssemblyStateCreateInfo inputAssembly{
            .sType      = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
            .topology   = toVkTopology(mSettings.topology),
        };

        // ── Rasterization ─────────────────────────────────────────────────────────
        auto toVkCull = [](CullMode m) -> VkCullModeFlags {
            switch (m) {
            case CullMode::None:  return VK_CULL_MODE_NONE;
            case CullMode::Front: return VK_CULL_MODE_FRONT_BIT;
            case CullMode::Back:  return VK_CULL_MODE_BACK_BIT;
            }
            return VK_CULL_MODE_BACK_BIT;
            };
        auto toVkPoly = [](PolygonMode m) -> VkPolygonMode {
            switch (m) {
            case PolygonMode::Fill:  return VK_POLYGON_MODE_FILL;
            case PolygonMode::Line:  return VK_POLYGON_MODE_LINE;
            case PolygonMode::Point: return VK_POLYGON_MODE_POINT;
            }
            return VK_POLYGON_MODE_FILL;
            };

        const VkPipelineRasterizationStateCreateInfo rasterization{
            .sType          = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
            .polygonMode    = toVkPoly(mSettings.polygonMode),
            .cullMode       = toVkCull(mSettings.cullMode),
            .frontFace      = VK_FRONT_FACE_COUNTER_CLOCKWISE,
            .lineWidth      = 1.0f,
        };

        // ── Multisample ───────────────────────────────────────────────────────────
        const VkPipelineMultisampleStateCreateInfo multisample{
            .sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
            .rasterizationSamples   = VK_SAMPLE_COUNT_1_BIT,
        };

        // ── Depth stencil ─────────────────────────────────────────────────────────
        const VkPipelineDepthStencilStateCreateInfo depthStencil{
            .sType              = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO,
            .depthTestEnable    = mSettings.depthTest ? VK_TRUE : VK_FALSE,
            .depthWriteEnable   = mSettings.depthWrite ? VK_TRUE : VK_FALSE,
            .depthCompareOp     = VK_COMPARE_OP_LESS_OR_EQUAL,
        };

        // ── Color blend (one attachment, no blending for now) ─────────────────────
        const VkPipelineColorBlendAttachmentState colorBlendAttachment{
            .colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                              VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
        };
        const VkPipelineColorBlendStateCreateInfo colorBlend{
            .sType              = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
            .attachmentCount    = 1,
            .pAttachments       = &colorBlendAttachment,
        };

        // ── Dynamic state (viewport/scissor динамические) ─────────────────────────
        constexpr VkDynamicState dynamicStates[] = {
            VK_DYNAMIC_STATE_VIEWPORT,
            VK_DYNAMIC_STATE_SCISSOR,
        };
        const VkPipelineDynamicStateCreateInfo dynamicState{
            .sType              = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
            .dynamicStateCount  = static_cast<uint32_t>(std::size(dynamicStates)),
            .pDynamicStates     = dynamicStates,
        };
        const VkPipelineViewportStateCreateInfo viewportState{
            .sType          = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
            .viewportCount  = 1,
            .scissorCount   = 1,
        };

        // ── Dynamic rendering (no render pass object) ─────────────────────────────
        const VkFormat colorFmt = ToVkFormat(mSettings.colorFormat);
        const VkFormat depthFmt = mSettings.depthFormat != ImageFormat::Undefined
            ? ToVkFormat(mSettings.depthFormat)
            : VK_FORMAT_UNDEFINED;

        VkPipelineRenderingCreateInfo renderingInfo{
            .sType                      = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO,
            .colorAttachmentCount       = 1,
            .pColorAttachmentFormats    = &colorFmt,
            .depthAttachmentFormat      = depthFmt,
        };

        // ── Assemble ──────────────────────────────────────────────────────────────
        const VkGraphicsPipelineCreateInfo pipelineInfo{
            .sType                  = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
            .pNext                  = &renderingInfo,
            .stageCount             = 2,
            .pStages                = stages,
            .pVertexInputState      = &vertexInput,
            .pInputAssemblyState    = &inputAssembly,
            .pViewportState         = &viewportState,
            .pRasterizationState    = &rasterization,
            .pMultisampleState      = &multisample,
            .pDepthStencilState     = &depthStencil,
            .pColorBlendState       = &colorBlend,
            .pDynamicState          = &dynamicState,
            .layout                 = mLayout,
        };

        VK_CHECK(vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &mPipeline));
        ENGINE_LOG_INFO("VulkanGraphicsPipeline", "Built");
    }

    void VulkanGraphicsPipeline::Destroy()
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