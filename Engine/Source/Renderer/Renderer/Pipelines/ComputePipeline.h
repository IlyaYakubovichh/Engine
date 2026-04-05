#pragma once
#include "Pipeline.h"

namespace Engine {

    class ENGINE_API ComputePipeline : public Pipeline {
    public:
        ~ComputePipeline() override = default;

        [[nodiscard]] PipelineType GetType() const override { return PipelineType::Compute; }

        [[nodiscard]] virtual const ComputePipelineSettings& GetSettings() const = 0;
    };

} // namespace Engine