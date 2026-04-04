#include "ShaderCompiler.h"
#include "File/Filesystem.h"
#include "Log/LogSystem.h"
#include <shaderc/shaderc.hpp>

namespace Engine {

    // ── Internal helpers ──────────────────────────────────────────────────────────

    /** Maps our ShaderStage to the shaderc equivalent. */
    static shaderc_shader_kind ToShadercKind(ShaderStage stage)
    {
        switch (stage) {
        case ShaderStage::Vertex:   return shaderc_glsl_vertex_shader;
        case ShaderStage::Fragment: return shaderc_glsl_fragment_shader;
        case ShaderStage::Compute:  return shaderc_glsl_compute_shader;
        case ShaderStage::Geometry: return shaderc_glsl_geometry_shader;
        }
        ENGINE_ASSERT_MSG(false, "Unknown ShaderStage");
        return shaderc_glsl_vertex_shader;
    }

    // ── Public API ────────────────────────────────────────────────────────────────

    std::vector<uint32_t> ShaderCompiler::CompileOrLoad(
        const std::filesystem::path& sourcePath,
        ShaderStage                  stage)
    {
        auto* fs = FileSystem::GetInstance();

        const auto cachePath = ResolveCachePath(sourcePath);

        // Cache hit
        if (fs->Exists(cachePath)) {
            ENGINE_LOG_INFO("ShaderCompiler", "Loading cached SPIR-V: {}", cachePath.string());
            return fs->ReadWords(cachePath);
        }

        // Cache miss: compile
        ENGINE_LOG_INFO("ShaderCompiler", "Compiling shader: {}", sourcePath.string());

        const std::string source = fs->ReadText(sourcePath);
        if (source.empty()) {
            ENGINE_LOG_ERROR("ShaderCompiler", "Source file is empty: {}", sourcePath.string());
            return {};
        }

        std::vector<uint32_t> spirv = CompileGLSL(source, sourcePath, stage);
        if (spirv.empty())
            return {};

        // Write cache
        fs->CreateDirectories(cachePath.parent_path());

        if (!fs->WriteWords(cachePath, spirv)) {
            ENGINE_LOG_WARN("ShaderCompiler", "Failed to write cache: {}", cachePath.string());
        }

        return spirv;
    }

    // ── Private ────────────────────────────────────────────────────────────────

    std::filesystem::path ShaderCompiler::ResolveCachePath(
        const std::filesystem::path& sourcePath)
    {
        // Cache/Shaders/Triangle.vert.spv
        auto* fs = FileSystem::GetInstance();
        const std::string filename = fs->GetFilename(sourcePath) + ".spv";
        return std::filesystem::path("Cache") / "Shaders" / filename;
    }

    std::vector<uint32_t> ShaderCompiler::CompileGLSL(
        const std::string&           source,
        const std::filesystem::path& sourcePath,
        ShaderStage                  stage)
    {
        shaderc::Compiler       compiler;
        shaderc::CompileOptions options;

        options.SetTargetEnvironment(shaderc_target_env_vulkan, shaderc_env_version_vulkan_1_4);
        options.SetOptimizationLevel(shaderc_optimization_level_performance);

        const auto result = compiler.CompileGlslToSpv(
            source,
            ToShadercKind(stage),
            sourcePath.string().c_str(),
            options);

        if (result.GetCompilationStatus() != shaderc_compilation_status_success) {
            ENGINE_LOG_ERROR("ShaderCompiler", "Compilation failed [{}]:\n{}", sourcePath.string(), result.GetErrorMessage());
            return {};
        }

        return { result.cbegin(), result.cend() };
    }

} // namespace Engine