#pragma once
#ifndef kaze_core_video_private_texture_h_
#define kaze_core_video_private_texture_h_

#include <kaze/core/lib.h>
#include <kaze/core/MemView.h>
#include <kaze/core/video/ImageHandle.h>
#include <kaze/core/video/PixelFormat.h>
#include <kaze/core/video/TextureHandle.h>

KAZE_NAMESPACE_BEGIN

namespace texture {
    /// Create a GPU texture from an image.
    /// \param[in]  image      the image
    /// \param[in]  freeImage  whether to free the image on successful conversion;
    ///                        note that if an invalid texture is returned, ownership
    ///                        remains with you, the caller.
    /// \returns generated texture handle, may be invalid on error
    auto fromImage(ImageHandle image, Bool freeImage) -> TextureHandle;

    /// Create a GPU texture from pixel data
    auto fromPixels(MemView<void> data, Size width, Size height,
        PixelFormat::Enum srcFormat) -> TextureHandle;

    /// Free a GPU texture
    /// \param[in]  texture    the texture handle to free
    auto free(TextureHandle texture) noexcept -> void;

    auto isValid(TextureHandle texture) noexcept -> Bool;
}

KAZE_NAMESPACE_END

#endif // kaze_core_video_private_texture_h_
