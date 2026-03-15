#pragma once
#include "Utility.h"
#include "Macro.h"
#include "Image.h"
#include <glm/glm.hpp>

namespace Engine {

	enum class API : uint8_t {
		None = LEFT_SHIFT(0),
		Vulkan = LEFT_SHIFT(1),
	};

	class ENGINE_API RendererAPI {
	public:
		static Ref<RendererAPI> Create(API api);

		virtual void Initialize() = 0;
		virtual void Shutdown() = 0;

		virtual void BeginFrame() = 0;
		virtual void EndFrame() = 0;
		virtual void BeginRenderPass(Ref<Image> renderTarget) = 0;
		virtual void EndRenderPass() = 0;
		virtual void Present() = 0;

		virtual void Clear(glm::vec4 clearColor) = 0;

		[[nodiscard]] const API& GetAPI() const { return mAPI; }

	protected:
		RendererAPI() = default;
		virtual ~RendererAPI() = default;

		API mAPI{ API::None };
	};

} // namespace Engine