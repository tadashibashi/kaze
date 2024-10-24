/// \file image.h
/// Contains low-level functions for handling loading images
#pragma once
#ifndef kaze_core_video_private_image_h_
#define kaze_core_video_private_iamge_h_

#include <kaze/core/lib.h>
#include <kaze/core/video/ImageHandle.h>
#include <kaze/core/video/PixelFormat.h>

KAZE_NAMESPACE_BEGIN

namespace image {
    /// Load image from file data
    /// \param[in]  data   data buffer
    /// \param[in]  size   number of bytes in data buffer
    /// \returns image data, make sure to call image::free on it.
    [[nodiscard]]
    auto load(const void *data, Size size) -> ImageHandle;

    /// Create an empty image, or one populated from const data
    /// \note Source format converts to an RGBA8 image, since its the internal software format for Kaze
    /// \param[in]  width      width of the image in pixels
    /// \param[in]  height     height of the image in pixels
    /// \param[in]  data       pixel data buffer; if not provided, image memory is set to 0 [optional]
    /// \param[in]  srcFormat  source pixel format of `data`; ignored if `data` is null [optional]
    [[nodiscard]]
    auto create(Size width, Size height,
                const void *data = nullptr,
                PixelFormat::Enum srcFormat = PixelFormat::RGBA8) -> ImageHandle;

    /// Make a copy of image data
    /// \param[in]  image   image to copy
    /// \returns image copy, you the user takes ownership of this handle
    [[nodiscard]]
    auto copy(const ImageHandle image) -> ImageHandle;

    /// Free image handle resources.
    auto free(ImageHandle image) -> void;
}

KAZE_NAMESPACE_END

#endif // kaze_core_video_private_iamge_h_