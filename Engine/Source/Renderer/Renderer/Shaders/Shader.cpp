#include "Shader.h"
#include "Shaders/VulkanShader.h"

namespace Engine {

    Ref<Shader> Shader::Create(const std::filesystem::path& path, ShaderStage stage)
    {
        return std::make_shared<VulkanShader>(path, stage);
    }

} // namespace Engine