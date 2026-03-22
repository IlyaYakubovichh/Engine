#pragma once

#include "Renderer/Shader.h"
#include "Macros.h"
#include <vulkan/vulkan.h>
#include <filesystem>
#include <vector>
#include <string>

namespace Engine {

    /**
     * @brief Vulkan backend implementation of Shader.
     *
     * Owns one VkShaderModule. Created from pre-compiled SPIR-V words.
     * VkShaderModule is destroyed on Free() or destructor, whichever comes first.
     */
    class ENGINE_API VulkanShader final : public Shader {
    public:
        /**
         * @param path   Path to the GLSL source file (cache is resolved internally).
         * @param stage  Pipeline stage this module belongs to.
         */
        VulkanShader(const std::filesystem::path& path, ShaderStage stage);
        ~VulkanShader() override;

        // ── Shader interface ──────────────────────────────────────────────────────
        [[nodiscard]] ShaderStage        GetStage() const override { return mStage; }
        [[nodiscard]] const std::string& GetName()  const override { return mName; }

        void Free() override;

        // ── Vulkan specific ───────────────────────────────────────────────────────

        /** Returns the raw VkShaderModule handle. Valid until Free() is called. */
        [[nodiscard]] VkShaderModule GetModule() const { return mModule; }

        /**
         * @brief Builds a VkPipelineShaderStageCreateInfo for this module.
         *
         * Convenience used by PipelineBuilder to avoid boilerplate.
         * entry is almost always "main".
         */
        [[nodiscard]] VkPipelineShaderStageCreateInfo GetStageInfo(
            const char* entry = "main") const;

    private:
        void CreateModule(const std::vector<uint32_t>& spirv);

    private:
        VkShaderModule mModule{ VK_NULL_HANDLE };
        ShaderStage    mStage{};
        std::string    mName;
    };

} // namespace Engine