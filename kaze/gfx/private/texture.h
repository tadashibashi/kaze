#pragma once
#include <kaze/gfx/lib.h>
#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/MemView.h>
#include <kaze/gfx/ImageHandle.h>
#include <kaze/gfx/PixelFormat.h>
#include <kaze/gfx/TextureHandle.h>

KGFX_NS_BEGIN

namespace texture {
    /// Create a GPU texture from an image.
    /// \param[in]  image      the image
    /// \param[in]  freeImage  whether to free the image on successful conversion;
    ///                        note that if an invalid texture is returned, ownership
    ///                        remains with you, the caller.
    /// \returns generated texture handle, may be invalid on error
    auto fromImage(ImageHandle image, Bool freeImage) -> TextureHandle;

    /// Create a GPU texture from pixel data
    auto fromPixels(MemView<void> data, Vec2<Uint> dimensions,
        PixelFormat::Enum srcFormat) -> TextureHandle;

    /// Free a GPU texture
    /// \param[in]  texture    the texture handle to free
    auto free(TextureHandle texture) noexcept -> void;

    auto isValid(TextureHandle texture) noexcept -> Bool;
}

KAZE_NS_END
