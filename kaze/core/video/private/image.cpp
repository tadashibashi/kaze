/// \file image.cpp
/// bimg::ImageContainer-related function implementation
#include "image.h"
#include "bimg/bimg.h"
#include <bimg/decode.h>
#include <bx/allocator.h>
#include <bx/error.h>

#include <kaze/core/debug.h>

KAZE_NAMESPACE_BEGIN

static bx::DefaultAllocator defaultAlloc{};

auto image::load(const void *data, const Size size) -> ImageHandle
{
    if ( !data )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required argument `data` was null");
        return {};
    }

    if (size == 0)
    {
        KAZE_CORE_ERRCODE(Error::InvalidArgErr, "size of file data passed to loadImage was 0");
        return {};
    }

    bx::Error error{};
    bimg::ImageContainer *result = nullptr;
    if ( result = bimg::imageParse(&defaultAlloc, data, size, bimg::TextureFormat::RGBA8, &error);
        result == nullptr )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bimg failed to parse image: {}",
            error.getMessage().getCPtr());
        return {};
    }

    return {.handle = result};
}


auto image::create(Size width, Size height,
                   const void *data, PixelFormat::Enum srcFormat) -> ImageHandle
{
    auto image = bimg::imageAlloc(&defaultAlloc, bimg::TextureFormat::RGBA8U,
            width, height, 0, 1, false, false, nullptr);
    if ( !image )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "Failed to allocate bimg::ImageContainer");
        return {};
    }

    if (data)
    {
        if ( !PixelFormat::toRGBA8((Ubyte *)image->m_data, (Ubyte *)data, width * height, srcFormat) )
        {
            bimg::imageFree(image);
            return {};
        }
    }

    return {.handle = image};
}


auto image::copy(ImageHandle imageHandle) -> ImageHandle
{
    auto image = static_cast<bimg::ImageContainer *>(imageHandle.handle);
    if ( !image )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "required arg `image` was null");
        return {};
    }

    auto newImage = bimg::imageAlloc(&defaultAlloc, image->m_format, image->m_width, image->m_height,
            image->m_depth, image->m_numLayers, image->m_cubeMap, image->m_numMips > 1,
            image->m_data);

    if ( !newImage )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bimg::imageAlloc failed");
    }

    return {.handle = newImage};
}


auto image::free(ImageHandle image) -> void
{
    if (image.isValid())
    {
        bimg::imageFree(static_cast<bimg::ImageContainer *>(image.handle));
    }
}

KAZE_NAMESPACE_END

