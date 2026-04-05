#pragma once
#include "Macros.h"
#include "Utils.h"
#include "Shaders/Shader.h"
#include "Resources/Image.h"

namespace Engine {

    // ─── Enums ────────────────────────────────────────────────────────────────────

    enum class PipelineType { Graphics, Compute };

    enum class PrimitiveTopology : uint8_t {
        TriangleList,
        TriangleStrip,
        LineList,
        PointList,
    };

    enum class CullMode : uint8_t {
        None,
        Front,
        Back,
    };

    enum class PolygonMode : uint8_t {
        Fill,
        Line,
        Point,
    };

    // ─── Settings ─────────────────────────────────────────────────────────────────

    struct ENGINE_API GraphicsPipelineSettings {
        Ref<Shader>       vertexShader;
        Ref<Shader>       fragmentShader;
        ImageFormat       colorFormat   = ImageFormat::RGBA8_Unorm;
        ImageFormat       depthFormat   = ImageFormat::Undefined;   // Undefined = no depth
        PrimitiveTopology topology      = PrimitiveTopology::TriangleList;
        CullMode          cullMode      = CullMode::Back;
        PolygonMode       polygonMode   = PolygonMode::Fill;
        bool              depthTest     = true;
        bool              depthWrite    = true;
        // future: std::vector<DescriptorSetLayout> layouts;
    };

    struct ENGINE_API ComputePipelineSettings {
        Ref<Shader> shader;
        // future: std::vector<DescriptorSetLayout> layouts;
    };

    // ─── Base ─────────────────────────────────────────────────────────────────────

    class ENGINE_API Pipeline : public NonCopyable {
    public:
        ~Pipeline() override = default;

        [[nodiscard]] virtual PipelineType GetType()  const = 0;
        [[nodiscard]] virtual bool         IsValid()  const = 0;

        // Factory — routes through PipelineLibrary (cache-aware)
        [[nodiscard]] static Ref<Pipeline> CreateGraphics(const GraphicsPipelineSettings& settings);
        [[nodiscard]] static Ref<Pipeline> CreateCompute(const ComputePipelineSettings& settings);
    };

} // namespace Engine