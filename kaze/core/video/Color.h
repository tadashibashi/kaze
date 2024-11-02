#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/endian.h>

KAZE_NS_BEGIN
#pragma pack(push, 1)
class Color
{
public:
    /// Default color is white
    constexpr Color() : r(255), g(255), b(255), a(255) { }

    /// Set rgba values individually
    constexpr Color(const Ubyte r, const Ubyte g, const Ubyte b, const Ubyte a = 255) : r(r), g(g), b(b), a(a) { }


    static constexpr Color fromGrayScale(const Ubyte gray, const Ubyte alpha = 255)
    {
        return {gray, gray, gray, alpha};
    }

    /// Conversion functions to and from unsigned 32-bit integers are named in human-readable byte order, so they assume
    /// that the data is in big-endian format. You'll need to use the opposite order function if it is in little-endian.

    /// Create a color from an unsigned integer in RGBA format (big-endian)
    /// \param[in] value    the unsigned 32-bit integer to parse
    [[nodiscard]] static constexpr Color fromRGBA8(const Uint value)
    {
        return {
            static_cast<Ubyte>((value & 0xFF000000u) >> 24u),
            static_cast<Ubyte>((value & 0x00FF0000u) >> 16u),
            static_cast<Ubyte>((value & 0x0000FF00u) >> 8u),
            static_cast<Ubyte>(value & 0x000000FFu),
        };
    }

    /// Create a color from an unsigned integer in ABGR format (big-endian)
    /// \param[in] value    the unsigned 32-bit integer to parse
    [[nodiscard]] static constexpr Color fromABGR8(const Uint value)
    {
        return {
            static_cast<Ubyte>(value & 0x000000FFu),
            static_cast<Ubyte>((value & 0x0000FF00u) >> 8u),
            static_cast<Ubyte>((value & 0x00FF0000u) >> 16u),
            static_cast<Ubyte>((value & 0xFF000000u) >> 24u),
        };
    }

    /// Create a color from an unsigned integer in ARGB format (big-endian)
    /// \param[in] value    the unsigned 32-bit integer to parse
    [[nodiscard]] static constexpr Color fromARGB8(const Uint value)
    {
        return {
            static_cast<Ubyte>((value & 0x00FF0000) >> 16u),
            static_cast<Ubyte>((value & 0x0000FF00) >> 8u),
            static_cast<Ubyte>(value & 0x000000FF),
            static_cast<Ubyte>((value & 0xFF000000) >> 24u),
        };
    }

    /// Create a color from an unsigned integer in BGRA format (big-endian)
    /// \param[in] value    the unsigned 32-bit integer to parse
    [[nodiscard]] static constexpr Color fromBGRA8(const Uint value)
    {
        return {
            static_cast<Ubyte>((value & 0x0000FF00) >> 8u),
            static_cast<Ubyte>((value & 0x00FF0000) >> 16u),
            static_cast<Ubyte>((value & 0xFF000000) >> 24u),
            static_cast<Ubyte>(value & 0x000000FF),
        };
    }

    Ubyte r, g, b, a;

    /// Set rgba values
    Color &set(const Ubyte red, const Ubyte green, const Ubyte blue, const Ubyte alpha = 255)
    {
        r = red;
        g = green;
        b = blue;
        a = alpha;
        return *this;
    }

    /// Set from grayscale value
    Color &setGrayScale(const Ubyte gray, const Ubyte alpha = 255)
    {
        r = gray;
        g = gray;
        b = gray;
        a = alpha;
        return *this;
    }

    /// Set red channel
    Color &setR(const Ubyte value) { r = value; return *this; }
    /// Set green channel
    Color &setG(const Ubyte value) { g = value; return *this; }
    /// Set blue channel
    Color &setB(const Ubyte value) { b = value; return *this; }
    /// Set alpha channel
    Color &setA(const Ubyte value) { a = value; return *this; }

    /// Get a pointer to the data array, useful for lower-level ops like sending data to the graphics card, etc.
    [[nodiscard]] const Ubyte *data() const { return &r; }
    /// Get a pointer to the data array, useful for lower-level ops like sending data to the graphics card, etc.
    [[nodiscard]] Ubyte *data() { return &r; }

    // Operators

    /// Get color data by index.
    /// In order of: r, g, b, a
    [[nodiscard]] const Ubyte &operator[](const Size byte) const
    {
        return (&r)[byte];
    }

    /// Get or set color data by index.
    /// In order of: r, g, b, a
    [[nodiscard]] Ubyte &operator[](const Size byte)
    {
        return (&r)[byte];
    }

    [[nodiscard]] bool operator==(const Color other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    [[nodiscard]] bool operator!=(const Color other) const
    {
        return !operator==(other);
    }

    // Basic color constants
    // ----- grays -----
    static const Color White; ///< pure white (255, 255, 255)
    static const Color Gray;  ///< half way gray (128, 128, 128)
    static const Color Black; ///< pure black (0, 0, 0)

    // ----- primary colors -----
    static const Color Red;   ///< pure red (255, 0, 0)
    static const Color Green; ///< pure green (0, 255, 0)
    static const Color Blue;  ///< pure blue (0, 0, 255)

    // ----- secondary colors -----
    static const Color Cyan;    ///< (0, 255, 255)
    static const Color Magenta; ///< (255, 0, 255)
    static const Color Yellow;  ///< (255, 255, 0)

    // ----- tertiary colors -----
    static const Color Orange;      ///< (255, 128, 0)
    static const Color Chartreuse;  ///< (128, 255, 0)
    static const Color SpringGreen; ///< (0, 255, 128)
    static const Color Azure;       ///< (0, 128, 255)
    static const Color Violet;      ///< (128, 0, 255)
    static const Color Rose;        ///< (255, 0, 128)

    /// Convert to an unsigned 32-bit integer in RGBA channel order, big-endian
    [[nodiscard]] constexpr Uint toRGBA8() const
    {
        return
            static_cast<Uint>(r) << 24u |
            static_cast<Uint>(g) << 16u |
            static_cast<Uint>(b) << 8u |
            static_cast<Uint>(a);
    }

    /// Concvert to an unsigned 32-bit integer in ABGR channel order, big-endian
    [[nodiscard]] constexpr Uint toABGR8() const
    {
        return
            static_cast<Uint>(a) << 24u |
            static_cast<Uint>(b) << 16u |
            static_cast<Uint>(g) << 8u |
            static_cast<Uint>(r);
    }

    /// Convert to an unsigned 32-bit integer in ARGB channel order, big-endian
    [[nodiscard]] constexpr Uint toARGB8() const
    {
        return
            static_cast<Uint>(a) << 24u |
            static_cast<Uint>(r) << 16u |
            static_cast<Uint>(g) << 8u |
            static_cast<Uint>(b);
    }

    /// Convert to an unsigned 32-bit integer in ARGB channel order, big-endian
    [[nodiscard]] constexpr Uint toBGRA8() const
    {
        return
            static_cast<Uint>(b) << 24u |
            static_cast<Uint>(g) << 16u |
            static_cast<Uint>(r) << 8u |
            static_cast<Uint>(a);
    }
};
#pragma pack(pop)

/// for British programmers
using Colour = Color;

KAZE_NS_END
