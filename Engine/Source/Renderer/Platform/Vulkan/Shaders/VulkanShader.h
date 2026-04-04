#pragma once

#include "Shaders/Shader.h"
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
        VulkanShader(const std::filesystem::path& path, ShaderStage stage);
        ~VulkanShader() override;

        [[nodiscard]] ShaderStage        GetStage() const override { return mStage; }
        [[nodiscard]] const std::string& GetName()  const override { return mName; }

        void Free() override;

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