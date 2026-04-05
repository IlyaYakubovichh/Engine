#pragma once
#include "Pipeline.h"

namespace Engine {

    class ENGINE_API GraphicsPipeline : public Pipeline {
    public:
        ~GraphicsPipeline() override = default;

        [[nodiscard]] PipelineType GetType() const override { return PipelineType::Graphics; }

        [[nodiscard]] virtual const GraphicsPipelineSettings& GetSettings() const = 0;
    };

} // namespace Engine