#pragma once
#include "Macros.h"
#include "Shader.h"
#include <filesystem>
#include <vector>
#include <cstdint>

namespace Engine {

    /**
     * @brief Compiles GLSL source files to SPIRV and manages it.
     *
     * If a cached .spv exists for the given source file, it is loaded directly.
     * Otherwise the source is compiled via shaderc and the result is written to cache.
     *
     * Cache location: Cache/Shaders/<filename>.spv  (relative to the working dir).
     */
    class ENGINE_API ShaderCompiler {
    public:
        /**
         * @brief Returns SPIR-V words for the given GLSL source file.
         *
         * Loads from cache if available, compiles and caches otherwise.
         * @param sourcePath  Path to the GLSL source file.
         * @param stage       Pipeline stage (Vertex, Fragment, …).
         * @return SPIR-V words. Empty on failure.
         */
        [[nodiscard]] static std::vector<uint32_t> CompileOrLoad(
            const std::filesystem::path& sourcePath,
            ShaderStage                  stage);

    private:
        /** Returns the cache path for a given source file. */
        [[nodiscard]] static std::filesystem::path ResolveCachePath(
            const std::filesystem::path& sourcePath);

        /** Compiles GLSL source text to SPIR-V words via shaderc. */
        [[nodiscard]] static std::vector<uint32_t> CompileGLSL(
            const std::string&           source,
            const std::filesystem::path& sourcePath,
            ShaderStage                  stage);
    };

} // namespace Engine