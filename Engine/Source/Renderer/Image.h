#pragma once

#include "Macro.h"
#include "Utility.h"
#include <memory>

namespace Engine {

    enum class ENGINE_API ImageUsage : uint32_t {
        None         = 0,
        RenderTarget = LEFT_SHIFT(0),
        Sampled      = LEFT_SHIFT(1),
        DepthStencil = LEFT_SHIFT(2),
        Storage      = LEFT_SHIFT(3),
        TransferSrc  = LEFT_SHIFT(4),
        TransferDst  = LEFT_SHIFT(5),
    };

    inline ImageUsage operator|(ImageUsage a, ImageUsage b) {
        return static_cast<ImageUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }
    inline ImageUsage operator&(ImageUsage a, ImageUsage b) {
        return static_cast<ImageUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }
    inline bool HasFlag(ImageUsage flags, ImageUsage flag) {
        return (static_cast<uint32_t>(flags) & static_cast<uint32_t>(flag)) != 0;
    }

    enum class ENGINE_API ImageFormat {
        Undefined,
        RGBA8_UNORM,
        BGRA8_UNORM,
        RGBA16_SFLOAT,
        RGBA32_SFLOAT,
        D32_SFLOAT,
        D24_UNORM_S8_UINT,
    };

    struct ENGINE_API ImageSpec {
        uint32_t    width       = 1;
        uint32_t    height      = 1;
        uint32_t    mipLevels   = 1;
        uint32_t    arrayLayers = 1;
        ImageFormat format      = ImageFormat::RGBA8_UNORM;
        ImageUsage  usage       = ImageUsage::None;
    };

	class ENGINE_API Image : public NonCopyable {
	public:
		virtual ~Image() = default;

		virtual uint32_t    GetWidth()      const = 0;
		virtual uint32_t    GetHeight()     const = 0;
		virtual uint32_t    GetMipLevels()  const = 0;
		virtual uint32_t    GetArrayLayers()const = 0;
		virtual ImageFormat GetFormat()     const = 0;
		virtual ImageUsage  GetUsage()      const = 0;

		bool IsDepthFormat()   const;
		bool IsColorFormat()   const;
		bool IsRenderTarget()  const;

		static Ref<Image> Create(const ImageSpec& spec);
	};

} // namespace Engine