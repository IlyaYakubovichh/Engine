#pragma once
#include "Macros.h"
#include "Singleton.h"
#include "Pipeline.h"
#include <unordered_map>

namespace Engine {

    class ENGINE_API PipelineLibrary final : public Singleton<PipelineLibrary> {
        friend class Singleton;
        friend class Pipeline; 

    public:
        static size_t HashGraphicsSettings(const GraphicsPipelineSettings& s)
        {
            size_t h = 0;
            auto combine = [&](size_t v) { h ^= v + 0x9e3779b9 + (h << 6) + (h >> 2); };
            combine(std::hash<void*>{}(s.vertexShader.get()));
            combine(std::hash<void*>{}(s.fragmentShader.get()));
            combine(std::hash<uint8_t>{}(static_cast<uint8_t>(s.colorFormat)));
            combine(std::hash<uint8_t>{}(static_cast<uint8_t>(s.depthFormat)));
            combine(std::hash<uint8_t>{}(static_cast<uint8_t>(s.topology)));
            combine(std::hash<uint8_t>{}(static_cast<uint8_t>(s.cullMode)));
            combine(std::hash<uint8_t>{}(static_cast<uint8_t>(s.polygonMode)));
            combine(std::hash<bool>{}(s.depthTest));
            combine(std::hash<bool>{}(s.depthWrite));
            return h;
        }

        static size_t HashComputeSettings(const ComputePipelineSettings& s)
        {
            return std::hash<void*>{}(s.shader.get());
        }

    public:
        void Invalidate(const GraphicsPipelineSettings& settings);
        void Invalidate(const ComputePipelineSettings& settings);
        void Clear();

    private:
        PipelineLibrary() = default;
        ~PipelineLibrary() override;

        [[nodiscard]] Ref<Pipeline> Find(size_t key) const;
        void                        Store(size_t key, Ref<Pipeline> pipeline);

        std::unordered_map<size_t, Ref<Pipeline>> mCache;
    };

} // namespace Engine