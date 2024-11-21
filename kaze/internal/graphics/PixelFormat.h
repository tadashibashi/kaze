#pragma once
#include <kaze/internal/graphics/lib.h>

KGFX_NS_BEGIN

/// Source pixel formats
struct PixelFormat {
    enum Enum {
        RGBA8,
        ARGB8,
        ABGR8,
        BGRA8,
        RGBX8,
        XRGB8,
        BGRX8,
        XBGR8,
        RGB8,
        BGR8,
    };

    /// Get stride for pixel format
    /// \param[in] format  the format to check
    /// \returns stride in bytes
    [[nodiscard]]
    static auto getStride(Enum format) noexcept -> Int;

    /// Convert a buffer of pixels from a source format to RGBA8, which is what kaze format uses
    /// \param[in]  dest        the destination pixel data buffer, must be separate from src
    /// \param[in]  src         the source pixel data buffer
    /// \param[in]  pixelCount  number of pixels to copy
    /// \param[in]  srcFormat   format of pixel data in the `src` buffer
    /// \returns whether conversion succeeded.
    static auto toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount, Enum srcFormat) -> Bool;
};

KGFX_NS_END
