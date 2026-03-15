#include "Image.h"		 
#include "VulkanRenderer/VulkanImage.h"

namespace Engine {

	bool Image::IsDepthFormat() const {
		return GetFormat() == ImageFormat::D32_SFLOAT ||
			   GetFormat() == ImageFormat::D24_UNORM_S8_UINT;
	}

	bool Image::IsColorFormat() const {
		return !IsDepthFormat();
	}

	bool Image::IsRenderTarget() const {
		return HasFlag(GetUsage(), ImageUsage::RenderTarget);
	}

	Ref<Image> Image::Create(const ImageSpec& spec) {
		auto image = std::make_shared<VulkanImage>();
		image->Initialize(spec);
		return image;
	}

} // namespace Engine