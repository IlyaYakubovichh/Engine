#include "Pipeline.h"
#include "PipelineLibrary.h"
#include "Core/RendererAPI.h"
#include "Log/LogSystem.h"

namespace Engine {

    Ref<Pipeline> Pipeline::CreateGraphics(const GraphicsPipelineSettings& settings)
    {
        ENGINE_ASSERT_MSG(settings.vertexShader, "Pipeline::CreateGraphics: vertex shader is null");
        ENGINE_ASSERT_MSG(settings.fragmentShader, "Pipeline::CreateGraphics: fragment shader is null");

        auto* lib = PipelineLibrary::GetInstance();
        const size_t key = PipelineLibrary::HashGraphicsSettings(settings);

        if (auto cached = lib->Find(key)) {
            ENGINE_LOG_DEBUG("Pipeline", "Cache hit (graphics)");
            return cached;
        }

        auto pipeline = RendererAPI::GetActive()->CreateGraphicsPipeline(settings);
        ENGINE_ASSERT_MSG(pipeline && pipeline->IsValid(), "Pipeline::CreateGraphics: creation failed");

        lib->Store(key, pipeline);
        ENGINE_LOG_INFO("Pipeline", "Graphics pipeline created and cached");
        return pipeline;
    }

    Ref<Pipeline> Pipeline::CreateCompute(const ComputePipelineSettings& settings)
    {
        ENGINE_ASSERT_MSG(settings.shader, "Pipeline::CreateCompute: shader is null");

        auto* lib = PipelineLibrary::GetInstance();
        const size_t key = PipelineLibrary::HashComputeSettings(settings);

        if (auto cached = lib->Find(key)) {
            ENGINE_LOG_DEBUG("Pipeline", "Cache hit (compute)");
            return cached;
        }

        auto pipeline = RendererAPI::GetActive()->CreateComputePipeline(settings);
        ENGINE_ASSERT_MSG(pipeline && pipeline->IsValid(), "Pipeline::CreateCompute: creation failed");

        lib->Store(key, pipeline);
        ENGINE_LOG_INFO("Pipeline", "Compute pipeline created and cached");
        return pipeline;
    }

} // namespace Engine