#include "ShaderLibrary.h"
#include "Log/LogSystem.h"

namespace Engine {

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        ENGINE_ASSERT_MSG(!Exists(name), "ShaderLibrary: shader is already registered");

        mShaders[name] = shader;
        ENGINE_LOG_INFO("ShaderLibrary", "Registered [{}]", name);
    }

    Ref<Shader> ShaderLibrary::Load(
        const std::string&           name,
        const std::filesystem::path& path,
        ShaderStage                  stage)
    {
        auto shader = Shader::Create(path, stage);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name) const
    {
        ENGINE_ASSERT_MSG(Exists(name), "ShaderLibrary: shader not found");

        return mShaders.at(name);
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return mShaders.contains(name);
    }

    void ShaderLibrary::Clear()
    {
        for (auto& [name, shader] : mShaders) {
            if (shader) shader->Free();
        }
        mShaders.clear();

        ENGINE_LOG_INFO("ShaderLibrary", "Cleared all shaders");
    }

    ShaderLibrary::~ShaderLibrary()
    {
        Clear();
    }

} // namespace Engine