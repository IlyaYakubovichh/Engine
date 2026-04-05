#include "PipelineLibrary.h"
#include "Log/LogSystem.h"

namespace Engine {

    Ref<Pipeline> PipelineLibrary::Find(size_t key) const
    {
        const auto it = mCache.find(key);
        return it != mCache.end() ? it->second : nullptr;
    }

    void PipelineLibrary::Store(size_t key, Ref<Pipeline> pipeline)
    {
        mCache[key] = std::move(pipeline);
    }

    void PipelineLibrary::Invalidate(const GraphicsPipelineSettings& settings)
    {
        mCache.erase(HashGraphicsSettings(settings));
        ENGINE_LOG_INFO("PipelineLibrary", "Invalidated graphics pipeline");
    }

    void PipelineLibrary::Invalidate(const ComputePipelineSettings& settings)
    {
        mCache.erase(HashComputeSettings(settings));
        ENGINE_LOG_INFO("PipelineLibrary", "Invalidated compute pipeline");
    }

    void PipelineLibrary::Clear()
    {
        mCache.clear();
        ENGINE_LOG_INFO("PipelineLibrary", "Cleared all pipelines");
    }

    PipelineLibrary::~PipelineLibrary()
    {
        Clear();
    }

} // namespace Engine