#pragma once
#ifndef kaze_core_video_image_h_
#define kaze_core_video_image_h_

#include "Color.h"
#include "ImageHandle.h"
#include "PixelFormat.h"

#include <kaze/gfx/lib.h>
#include <kaze/core/MemView.h>
#include <kaze/core/math/Vec/Vec2.h>

KGFX_NS_BEGIN

/// RAII container for pixel data, with loading functionality.
/// Stores pixels in RGBA8 format.
class Image {
public:
    Image() noexcept;
    ~Image() noexcept;

    /// Copying this class copies internal pixel data like a regular container
    Image(const Image &other) noexcept;
    /// Copying this class copies internal pixel data like a regular container
    auto operator=(const Image &other) noexcept -> Image &;
    Image(Image &&other) noexcept;
    auto operator=(Image &&other) noexcept -> Image &;

    /// \returns whether surface is currently loaded
    [[nodiscard]]
    auto isLoaded() const noexcept -> Bool;

    /// Load surface from a file
    /// \param[in] filepath   path of file to load
    /// \returns whether operation was successful
    auto load(StringView filepath) -> Bool;

    /// Load surface from const memory
    /// \param[in] memory   memory of file data to load surface from
    /// \returns whether operation was successful
    auto load(MemView<void> memory) -> Bool;

    /// Load a surface from pixel data. Copies this data internally.
    /// \param[in] pixels      pixel data to load surface from
    /// \param[in] width       width of the data to load in pixels
    /// \param[in] height      height of the data to load in pixels
    /// \param[in] srcFormat   format of the data in `pixels`
    /// \returns whether operation was successful
    auto loadPixels(MemView<void> pixels, Size width, Size height, PixelFormat::Enum srcFormat) -> Bool;

    /// Load surface from a list of pixels from left-to-right, top-to-bottom order.
    /// \param[in]  pixels      pixels to load
    /// \param[in]  width       pixel width of image data
    /// \param[in]  height      pixel height of image data
    /// \returns whether operation was successful
    auto loadPixels(MemView<Color> pixels, Size width, Size height) -> Bool
    {
        return loadPixels(pixels, width, height, PixelFormat::RGBA8);
    }

    /// Unload any currently loaded image data. Safe to call even if not loaded.
    auto unload() -> void;

    /// Dimensions in pixels
    [[nodiscard]]
    auto dimensions() const noexcept -> Vec2<Uint>;

    /// Number of total pixels in the surface
    [[nodiscard]]
    auto pixelCount() const noexcept -> Size;

    /// Get the pointer to the pixel data. Will return `nullptr` if not loaded.
    [[nodiscard]]
    auto data() const noexcept -> const Ubyte *;

    /// Get the pointer to the pixel data. Will return `nullptr` if not loaded.
    [[nodiscard]]
    auto data() noexcept -> Ubyte *;

    /// Total byte size of all the internal pixel data
    [[nodiscard]]
    auto size() const noexcept -> Size;

    /// Number of Mips
    [[nodiscard]]
    auto mipCount() const noexcept -> Size;

    /// Number of layers
    [[nodiscard]]
    auto layerCount() const noexcept -> Size;

    /// Whether image data is a cube map
    [[nodiscard]]
    auto isCubeMap() const noexcept -> Bool;

    auto handle() const noexcept -> const ImageHandle { return m_image; }
private:
    ImageHandle m_image;
};

KAZE_NS_END

#endif
