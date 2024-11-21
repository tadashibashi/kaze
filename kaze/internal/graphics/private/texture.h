#pragma once
#include <kaze/internal/graphics/lib.h>
#include <kaze/internal/core/math/Vec/Vec2.h>
#include <kaze/internal/core/MemView.h>
#include <kaze/internal/graphics/ImageHandle.h>
#include <kaze/internal/graphics/PixelFormat.h>
#include <kaze/internal/graphics/TextureHandle.h>

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

KGFX_NS_END
