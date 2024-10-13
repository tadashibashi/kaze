/// @file Texture2D
/// Texture2D class
#pragma once
#ifndef kaze_video_texture2d_h_
#define kaze_video_texture2d_h_

#include <kaze/kaze.h>
#include <kaze/math/Vec/Vec2.h>
#include <kaze/video/Color.h>

KAZE_NAMESPACE_BEGIN

/// RAII container around a GPU texture
class Texture2D
{
public:
    Texture2D();
    ~Texture2D();

    KAZE_NO_COPY(Texture2D);

    // move
    Texture2D(Texture2D &&other) noexcept;
    auto operator=(Texture2D &&other) noexcept -> Texture2D &;

    /// Load texture in constructor from const data
    /// @param[in]  data         pointer to the data
    /// @param[in]  byteLength   size of the data buffer in bytes
    Texture2D(const void *data, Size byteLength);

    /// Load texture in constructor from file path
    /// @param[in]  path    path to load texture from
    explicit Texture2D(Cstring path);

    /// Load texture from file data loaded into const memory
    /// @param[in] data   pointer to image data
    /// @param[in] size   size of data in bytes
    /// @returns whether operation was successful
    auto load(const void *data, Size size) -> Bool;

    /// Load texture from a file
    /// @param[in]  path   path to the texture file to load from
    /// @returns whether operation was successful
    auto load(Cstring path) -> Bool;

    /// Load texture from a file
    /// @param[in]  path    path to the texture file to load from
    /// @returns whether operation was successful
    auto load(const StringView path) -> Bool
    {
        return load(static_cast<Cstring>( path.data() ));
    }

    /// Load texture from a list of pixels from left-to-right, top-to-bottom order.
    /// @param[in]  pixels      pixel array, containing each Color
    /// @param[in]  pixelCount  number of pixels in the array (must total `width * height`)
    /// @param[in]  width       pixel width of image data
    /// @param[in]  height      pixel height of image data
    /// @returns whether operation was successful
    auto loadPixels(const Color *pixels, Size pixelCount, Size width, Size height) -> Bool;

    /// Check if a texture is currently loaded and ready for use.
    [[nodiscard]]
    auto isLoaded() const noexcept -> Bool;

    /// Free internal resources
    auto release() -> void;

    /// Get the GPU texture id
    [[nodiscard]]
    auto id() const noexcept -> Uint16 { return m_textureId; }

    /// Get the image's pixel dimensions;
    /// an unloaded image will result in {0, 0}
    [[nodiscard]]
    auto size() const noexcept -> Vec2i { return m_size; }

private:
    Uint16 m_textureId; ///< GPU texture id
    Vec2i m_size;
};

KAZE_NAMESPACE_END

#endif // kaze_video_texture2d_h_
