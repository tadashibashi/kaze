/// \file Texture2D
/// Texture2D class
#pragma once
#ifndef kaze_core_video_texture2d_h_
#define kaze_core_video_texture2d_h_

#include "PixelFormat.h"
#include "TextureHandle.h"

#include <kaze/core/lib.h>
#include <kaze/core/MemView.h>
#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/video/Color.h>

KAZE_NAMESPACE_BEGIN

class Image;

/// RAII container around a GPU texture
class Texture2D
{
public:
    Texture2D();
    ~Texture2D();

    Texture2D(const Texture2D &other);
    auto operator=(const Texture2D &other) -> Texture2D &;

    // move
    Texture2D(Texture2D &&other) noexcept;
    auto operator=(Texture2D &&other) noexcept -> Texture2D &;

    /// Load texture in constructor from const data
    /// \param[in]  data         pointer to the data
    /// \param[in]  byteLength   size of the data buffer in bytes
    Texture2D(const void *data, Size byteLength);

    /// Load texture in constructor from file path
    /// \param[in]  path    path to load texture from
    explicit Texture2D(Cstring path);

    /// Load texture from file data loaded into const memory
    /// \param[in] data   pointer to image data
    /// \param[in] size   size of data in bytes
    /// \returns whether operation was successful
    auto load(const void *data, Size size) -> Bool;

    /// Load texture from a file
    /// \param[in]  path   path to the texture file to load from
    /// \returns whether operation was successful
    auto load(Cstring path) -> Bool;

    /// Load texture from a file
    /// \param[in]  path    path to the texture file to load from
    /// \returns whether operation was successful
    auto load(const StringView path) -> Bool
    {
        return load(static_cast<Cstring>( path.data() ));
    }

    /// Load texture from pixel data
    /// \param[in]  pixels      pixels to load
    /// \param[in]  dimensions  pixel dimensions of image data
    /// \returns whether operation was successful
    auto loadPixels(MemView<Color> pixels, Vec2<Uint> dimensions) -> Bool;

    /// Load texture from pixel data
    /// \param[in]  data        pixel data to load
    /// \param[in]  dimensions  pixel dimensions of image data
    /// \param[in]  srcFormat   format of the data in `data`
    /// \returns whether operation was successful
    auto loadPixels(MemView<void> data, Vec2<Uint> dimensions, PixelFormat::Enum srcFormat) -> Bool;

    /// Load texture from an Image container
    /// \param[in]  image   container with pixel data to load
    /// \returns whether operation was successful
    auto loadImage(const Image &image) -> Bool;

    /// Check if a texture is currently loaded and ready for use.
    [[nodiscard]]
    auto isLoaded() const noexcept -> Bool;

    /// Free internal resources
    auto release() -> void;

    /// Get the TextureHandle
    [[nodiscard]]
    auto handle() const noexcept -> TextureHandle { return m_texture; }

    /// Get the image's pixel dimensions;
    /// an unloaded image will result in {0, 0}
    [[nodiscard]]
    auto size() const noexcept -> Vec2<Uint> { return m_size; }
private:
    TextureHandle m_texture; ///< GPU texture id
    Vec2<Uint> m_size;
};

KAZE_NAMESPACE_END

#endif // kaze_core_video_texture2d_h_
