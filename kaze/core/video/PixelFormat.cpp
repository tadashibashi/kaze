#include "PixelFormat.h"
#include <kaze/core/memory.h>

KAZE_NAMESPACE_BEGIN

Int PixelFormat::getStride(Enum format) noexcept
{
    switch(format)
    {
    case RGBA8: case ARGB8: case ABGR8: case BGRA8:
    case RGBX8: case XRGB8: case BGRX8: case XBGR8:
        return 4;
    case RGB8: case BGR8:
        return 3;
    default:
        return 0;
    }
}

// ===== Conversion functions =====

/// \note Okay to use the same pointer for both dest and src!
static auto ARGB8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t a = src[i * 4 + 0]; // Alpha
        uint8_t r = src[i * 4 + 1]; // Red
        uint8_t g = src[i * 4 + 2]; // Green
        uint8_t b = src[i * 4 + 3]; // Blue

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = a; // Alpha
    }
}

/// \note Okay to use the same pointer for both dest and src!
static auto ABGR8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t a = src[i * 4 + 0]; // Alpha
        uint8_t b = src[i * 4 + 1]; // Blue
        uint8_t g = src[i * 4 + 2]; // Green
        uint8_t r = src[i * 4 + 3]; // Red

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = a; // Alpha
    }
}

/// \note Okay to use the same pointer for both dest and src!
static auto BGRA8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t b = src[i * 4 + 0]; // Blue
        uint8_t g = src[i * 4 + 1]; // Green
        uint8_t r = src[i * 4 + 2]; // Red
        uint8_t a = src[i * 4 + 3]; // Alpha

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = a; // Alpha
    }
}

/// \note Okay to use the same pointer for both dest and src!
static auto RGBX8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t b = src[i * 4 + 0]; // Blue
        uint8_t g = src[i * 4 + 1]; // Green
        uint8_t r = src[i * 4 + 2]; // Red
        uint8_t a = 255;

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = a; // Alpha
    }
}

/// \note Okay to use the same pointer for both dest and src!
static auto XRGB8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t a = 255;
        uint8_t r = src[i * 4 + 1]; // Red
        uint8_t g = src[i * 4 + 2]; // Green
        uint8_t b = src[i * 4 + 3]; // Blue

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = a; // Alpha
    }
}

/// \note Okay to use the same pointer for both dest and src!
static auto BGRX8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t b = src[i * 4 + 0]; // Blue
        uint8_t g = src[i * 4 + 1]; // Green
        uint8_t r = src[i * 4 + 2]; // Red
        uint8_t a = 255;

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = a; // Alpha
    }
}

/// \note Okay to use the same pointer for both dest and src!
static auto XBGR8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t a = 255;
        uint8_t b = src[i * 4 + 1]; // Blue
        uint8_t g = src[i * 4 + 2]; // Green
        uint8_t r = src[i * 4 + 3]; // Red

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = a; // Alpha
    }
}

/// \note src and dest must be different
static auto RGB8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    KAZE_ASSERT(src != dest, "dest and src must not point to the same memory");
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t r = src[i * 3 + 0]; // Blue
        uint8_t g = src[i * 3 + 1]; // Green
        uint8_t b = src[i * 3 + 2]; // Red

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = 255;
    }
}

/// \note src and dest must be different
static auto BGR8toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount) -> void
{
    KAZE_ASSERT(src != dest, "dest and src must not point to the same memory");
    for (uint32_t i = 0; i < pixelCount; ++i)
    {
        uint8_t b = src[i * 3 + 0]; // Blue
        uint8_t g = src[i * 3 + 1]; // Green
        uint8_t r = src[i * 3 + 2]; // Red

        dest[i * 4 + 0] = r; // Red
        dest[i * 4 + 1] = g; // Green
        dest[i * 4 + 2] = b; // Blue
        dest[i * 4 + 3] = 255;
    }
}

auto PixelFormat::toRGBA8(Ubyte *dest, const Ubyte *src, Size pixelCount, PixelFormat::Enum srcFormat) -> Bool
{
    switch(srcFormat)
    {
        case PixelFormat::RGBA8: memory::copy(dest, src, 4 * pixelCount); break;
        case PixelFormat::ARGB8: ARGB8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::ABGR8: ABGR8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::BGRA8: BGRA8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::RGBX8: RGBX8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::XRGB8: XRGB8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::BGRX8: BGRX8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::XBGR8: XBGR8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::RGB8: RGB8toRGBA8(dest, src, pixelCount); break;
        case PixelFormat::BGR8: BGR8toRGBA8(dest, src, pixelCount); break;
        default:
            return false;
    }

    return true;
}

KAZE_NAMESPACE_END
