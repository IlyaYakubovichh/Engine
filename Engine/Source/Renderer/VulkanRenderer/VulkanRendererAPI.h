#pragma once
#include "RendererAPI.h"
#include "VulkanRendererData.h"
#include "Macro.h"
#include <glm/glm.hpp>

namespace Engine {

	class ENGINE_API VulkanRendererAPI final : public RendererAPI {
	public:
		VulkanRendererAPI();
		~VulkanRendererAPI() override = default;

		void Initialize() override;
		void Shutdown()   override;

		void BeginFrame() override;
		void EndFrame()   override;
		void BeginRenderPass(Ref<Image> renderTarget) override;
		void EndRenderPass()  override;
		void Present() override;

		void Clear(glm::vec4 clearColor) override;

	private:
		VulkanRendererData mData;
	};

} // namespace Engine