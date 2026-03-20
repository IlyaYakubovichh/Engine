#pragma once
#include "Macros.h"
#include "Utils.h"

namespace Engine {

    // ─── ImageUsage ───────────────────────────────────────────────────────────────

    enum class ENGINE_API ImageUsage : uint32_t {
        None            = 0,
        RenderTarget    = BIT_LEFT(0),
        Sampled         = BIT_LEFT(1),
        DepthStencil    = BIT_LEFT(2),
        Storage         = BIT_LEFT(3),
        TransferSrc     = BIT_LEFT(4),
        TransferDst     = BIT_LEFT(5),
    };

    inline ImageUsage operator|(ImageUsage a, ImageUsage b) noexcept
    {
        return static_cast<ImageUsage>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
    }

    inline ImageUsage operator&(ImageUsage a, ImageUsage b) noexcept
    {
        return static_cast<ImageUsage>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
    }

    // Returns true if @p flags contains @p flag.
    inline bool HasFlag(ImageUsage flags, ImageUsage flag) noexcept
    {
        return (flags & flag) != ImageUsage::None;
    }

    // ─── ImageFormat ──────────────────────────────────────────────────────────────

    enum class ENGINE_API ImageFormat : uint8_t {
        Undefined,
        RGBA8_Unorm,
        BGRA8_Unorm,
        RGBA16_Sfloat,
        RGBA32_Sfloat,
        D32_Sfloat,
        D24_Unorm_S8_Uint,
    };

    // ─── ImageSpec ────────────────────────────────────────────────────────────────

    struct ENGINE_API ImageSpec {
        uint32_t    width{ 1 };
        uint32_t    height{ 1 };
        uint32_t    mipLevels{ 1 };
        uint32_t    arrayLayers{ 1 };
        ImageFormat format{ ImageFormat::RGBA8_Unorm };
        ImageUsage  usage{ ImageUsage::None };
    };

    // ─── Image ────────────────────────────────────────────────────────────────────

    /**
     * @brief Abstract GPU image interface.
     *
     * Concrete implementations (e.g. VulkanImage) are created via Image::Create().
     * Lifetime is managed through Ref<Image>.
     * Call Free() to explicitly release GPU resources before the object is destroyed.
     */
    class ENGINE_API Image : public NonCopyable {
    public:
        ~Image() override = default;

        [[nodiscard]] virtual uint32_t    GetWidth()       const = 0;
        [[nodiscard]] virtual uint32_t    GetHeight()      const = 0;
        [[nodiscard]] virtual uint32_t    GetMipLevels()   const = 0;
        [[nodiscard]] virtual uint32_t    GetArrayLayers() const = 0;
        [[nodiscard]] virtual ImageFormat GetFormat()      const = 0;
        [[nodiscard]] virtual ImageUsage  GetUsage()       const = 0;

        // Explicitly releases GPU resources. Safe to call multiple times.
        virtual void Free() = 0;

        [[nodiscard]] bool IsDepthFormat()  const;
        [[nodiscard]] bool IsColorFormat()  const;
        [[nodiscard]] bool IsRenderTarget() const;

        // Creates a backend-specific image. Currently always returns a VulkanImage.
        [[nodiscard]] static Ref<Image> Create(const ImageSpec& spec);
    };

} // namespace Engine