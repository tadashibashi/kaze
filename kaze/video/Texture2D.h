/// @file Texture2D
/// Texture2D class
#pragma once
#ifndef kaze_video_texture2d_h_
#define kaze_video_texture2d_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

class Color;

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
    /// @param[in] data         pointer to the data
    /// @param[in] byteLength   size of the data buffer in bytes
    Texture2D(const void *data, Size byteLength);

    /// Load texture in constructor from file path
    /// @param[in] path path to load texture from
    explicit Texture2D(Cstring path);

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

    /// Load texture from file data loaded into const memory
    /// @param[in] data         pointer to image data
    /// @param[in] byteLength   size of data in bytes
    /// @returns whether operation was successful
    auto load(const void *data, Size byteLength) -> Bool;

    /// Load texture from a list of pixels from left-to-right, top-to-bottom order.
    /// @param[in]  pixels      pixel array, containing each Color
    /// @param[in]  pixelCount  number of pixels in the array (must total `width * height`)
    /// @param[in]  width       pixel width of image data
    /// @param[in]  height      pixel height of image data
    /// @returns whether operation was successful
    auto loadPixels(const Color *pixels, Size pixelCount, Size width, Size height) -> Bool;

    /// Check if a texture is currently loaded and ready for use.
    auto isLoaded() const noexcept -> Bool;

    /// Free internal resources
    auto release() -> void;

    /// Get the GPU texture id
    [[nodiscard]]
    auto id() const noexcept -> Uint16 { return m_textureId; }
private:
    Uint16 m_textureId;
};

KAZE_NAMESPACE_END

#endif // kaze_video_texture2d_h_