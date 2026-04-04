#include "Image.h"
#include "Resources/VulkanImage.h"

namespace Engine {

    bool Image::IsDepthFormat() const
    {
        return GetFormat() == ImageFormat::D32_Sfloat ||
            GetFormat() == ImageFormat::D24_Unorm_S8_Uint;
    }

    bool Image::IsColorFormat()  const { return !IsDepthFormat(); }
    bool Image::IsRenderTarget() const { return HasFlag(GetUsage(), ImageUsage::RenderTarget); }

    Ref<Image> Image::Create(const ImageSpec& spec)
    {
        return std::make_shared<VulkanImage>(spec);
    }

} // namespace Engine