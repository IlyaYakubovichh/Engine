#include "Shader.h"
#include "Vulkan/VulkanShader.h"

namespace Engine {

    Ref<Shader> Shader::Create(const std::filesystem::path& path, ShaderStage stage)
    {
        return std::make_shared<VulkanShader>(path, stage);
    }

} // namespace Engine