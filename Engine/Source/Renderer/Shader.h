#pragma once
#include "Macros.h"
#include "Utils.h"
#include <filesystem>
#include <string>

namespace Engine {

    /** Identifies which pipeline stage a shader belongs to. */
    enum class ENGINE_API ShaderStage {
        Vertex,
        Fragment,
        Compute,
        Geometry,
    };

    /**
     * @brief Backend-agnostic shader handle.
     *
     * Wraps a single compiled shader stage (one SPIR-V module in Vulkan).
     * Create via Shader::Create(); the active backend decides the implementation.
     * Call Free() to explicitly release GPU resources before the object is destroyed.
     */
    class ENGINE_API Shader : public NonCopyable {
    public:
        ~Shader() override = default;

        [[nodiscard]] virtual ShaderStage        GetStage() const = 0;
        [[nodiscard]] virtual const std::string& GetName()  const = 0;

        // Explicitly releases GPU resources. Safe to call multiple times.
        virtual void Free() = 0;

        /**
         * @brief Factory. Creates the concrete backend shader.
         * @param path   Path to the SPIR-V binary (.spv file).
         * @param stage  Which pipeline stage this module belongs to.
         */
        [[nodiscard]] static Ref<Shader> Create(const std::filesystem::path& path, ShaderStage stage);

    protected:
        Shader() = default;
    };

} // namespace Engine