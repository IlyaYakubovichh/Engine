#include "VulkanShader.h"
#include "ShaderCompiler.h"
#include "Systems/VulkanSystem/VulkanSystem.h"
#include "Systems/FileSystem/FileSystem.h"
#include "Systems/LogSystem/LogSystem.h"
#include "VulkanUtils.h"

namespace Engine {

    // ── Lifecycle ─────────────────────────────────────────────────────────────────

    VulkanShader::VulkanShader(const std::filesystem::path& path, ShaderStage stage)
        : mStage(stage)
    {
        auto* fs = FileSystem::GetInstance();
        mName = fs->GetFilename(path);

        const auto spirv = ShaderCompiler::CompileOrLoad(path, stage);
        ENGINE_ASSERT_MSG(!spirv.empty(), "Failed to compile/load shader");

        CreateModule(spirv);

        ENGINE_LOG_INFO("VulkanShader", "Created [{}]", mName);
    }

    VulkanShader::~VulkanShader()
    {
        Free();
    }

    // ── Shader interface ──────────────────────────────────────────────────────────

    void VulkanShader::Free()
    {
        if (mModule == VK_NULL_HANDLE)
            return;

        const auto device = VulkanSystem::GetInstance()->GetVkDevice();
        vkDestroyShaderModule(device, mModule, nullptr);
        mModule = VK_NULL_HANDLE;

        ENGINE_LOG_INFO("VulkanShader", "Freed [{}]", mName);
    }

    // ── Vulkan specific ───────────────────────────────────────────────────────────

    VkPipelineShaderStageCreateInfo VulkanShader::GetStageInfo(const char* entry) const
    {
        // Maps ShaderStage to VkShaderStageFlagBits
        static auto toVkStage = [](ShaderStage s) -> VkShaderStageFlagBits {
            switch (s) {
            case ShaderStage::Vertex:   return VK_SHADER_STAGE_VERTEX_BIT;
            case ShaderStage::Fragment: return VK_SHADER_STAGE_FRAGMENT_BIT;
            case ShaderStage::Compute:  return VK_SHADER_STAGE_COMPUTE_BIT;
            case ShaderStage::Geometry: return VK_SHADER_STAGE_GEOMETRY_BIT;
            }
            return VK_SHADER_STAGE_VERTEX_BIT;
        };

        VkPipelineShaderStageCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        info.stage = toVkStage(mStage);
        info.module = mModule;
        info.pName = entry;
        return info;
    }

    // ── Private helpers ───────────────────────────────────────────────────────────

    void VulkanShader::CreateModule(const std::vector<uint32_t>& spirv)
    {
        VkShaderModuleCreateInfo info{};
        info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        info.codeSize = spirv.size() * sizeof(uint32_t);
        info.pCode = spirv.data();

        const auto device = VulkanSystem::GetInstance()->GetVkDevice();
        VK_CHECK(vkCreateShaderModule(device, &info, nullptr, &mModule));
    }

} // namespace Engine