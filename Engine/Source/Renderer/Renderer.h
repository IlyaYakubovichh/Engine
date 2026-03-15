#pragma once
#include "RendererAPI.h"
#include "Image.h"
#include "Macro.h"
#include "Utility.h"
#include <glm/glm.hpp>

namespace Engine {

	struct RendererSettings {
		API rendererAPI{ API::None };
	};

	class ENGINE_API Renderer final {
	public:
		static void Initialize(const RendererSettings& settings);
		static void Shutdown();

		static void BeginFrame();
		static void EndFrame();
		static void BeginRenderPass(Ref<Image> renderTarget);
		static void EndRenderPass();
		static void Present();

		static void Clear(glm::vec4 clearColor);

		[[nodiscard]] static const API& GetAPI() { return sRendererAPI->GetAPI(); }
		[[nodiscard]] static const RendererSettings& GetRendererSettings() { return sRendererSettings; }

	private:
		Renderer() = default;
		~Renderer() = default;

		static Ref<RendererAPI>  sRendererAPI;
		static RendererSettings  sRendererSettings;
	};

} // namespace Engine