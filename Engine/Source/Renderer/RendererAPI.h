#pragma once
#include "Macros.h"
#include "Utils.h"
#include "Image.h"
#include <glm/glm.hpp>

namespace Engine {

    /**
     * @brief Abstract rendering backend interface.
     *
     * One concrete implementation exists per backend (e.g. VulkanRendererAPI).
     * Created via RendererAPI::Create().
     */
    class ENGINE_API RendererAPI {
    public:
        virtual ~RendererAPI() = default;

        virtual void Initialize()       = 0;
        virtual void Shutdown()         = 0;

        virtual void BeginFrame()       = 0;
        virtual void EndFrame()         = 0;
        virtual void BeginRenderPass()  = 0;
        virtual void EndRenderPass()    = 0;
        virtual void Present()          = 0;

        virtual void SetRenderTarget(Ref<Image> target) = 0;
        virtual void Clear(glm::vec4 clearColor)        = 0;

        // Creates the active backend API instance.
        [[nodiscard]] static Ref<RendererAPI> Create();

    protected:
        RendererAPI() = default;
    };

} // namespace Engine