#pragma once

#include <kaze/core/lib.h>

KAZE_NS_BEGIN

struct ImageHandle {
    void *handle{};

    operator Bool() const noexcept {
        return static_cast<Bool>(handle);
    }

    auto isValid() const noexcept -> Bool {
        return static_cast<Bool>(handle);
    }

    /// \returns pointer to RGBA8-formatted pixel data
    [[nodiscard]]
    auto data() const noexcept -> const Ubyte *;

    /// \returns pointer to RGBA8-formatted pixel data
    [[nodiscard]]
    auto data() noexcept -> Ubyte *;

    /// \returns image pixel width
    [[nodiscard]]
    auto width() const noexcept -> Uint;

    /// \returns image pixel height
    [[nodiscard]]
    auto height() const noexcept -> Uint;

    /// \returns number of pixels in the image
    [[nodiscard]]
    auto pixelCount() const noexcept -> Size;

    /// \returns total byte size of all the internal pixel data
    [[nodiscard]]
    auto size() const noexcept -> Size;

    /// \returns the number of mip maps
    [[nodiscard]]
    auto mipCount() const noexcept -> Size;

    /// \returns the number of image layers
    [[nodiscard]]
    auto layerCount() const noexcept -> Size;

    /// \returns whether image data is a cube map
    [[nodiscard]]
    auto isCubeMap() const noexcept -> Bool;
};

KAZE_NS_END
