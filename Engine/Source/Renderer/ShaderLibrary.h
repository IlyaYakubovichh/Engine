#pragma once

#include "Macros.h"
#include "Singleton.h"
#include "Shader.h"
#include <unordered_map>
#include <filesystem>
#include <string>

namespace Engine {

    /**
     * @brief Global name-to-shader registry.
     *
     * Shaders are loaded once and stored here by name.
     * Pipelines and materials look up their stages from this library
     * rather than holding raw Shader pointers themselves.
     *
     * Usage:
     *   ShaderLibrary::GetInstance()->Add("MyVert", vertShader);
     *   auto s = ShaderLibrary::GetInstance()->Get("MyVert");
     */
    class ENGINE_API ShaderLibrary final : public Singleton<ShaderLibrary> {
        friend class Singleton;
    public:
        /** Registers a shader under the given name. Asserts if name already exists. */
        void Add(const std::string& name, const Ref<Shader>& shader);

        /**
         * @brief Loads a SPIR-V file, creates a shader, and registers it in one call.
         * @return The newly created shader.
         */
        Ref<Shader> Load(const std::string& name, const std::filesystem::path& path, ShaderStage stage);

        /** Returns the shader registered under name. Asserts if not found. */
        [[nodiscard]] Ref<Shader> Get(const std::string& name) const;

        /** Returns true if a shader with the given name is registered. */
        [[nodiscard]] bool Exists(const std::string& name) const;

        /** Destroys all shader modules and clears the registry. */
        void Clear();

    private:
        ShaderLibrary() = default;
        ~ShaderLibrary() override = default;

        std::unordered_map<std::string, Ref<Shader>> mShaders;
    };

} // namespace Engine