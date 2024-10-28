#include "texture.h"

#include <kaze/core/debug.h>
#include <kaze/core/memory.h>
#include <kaze/core/traits.h>

#include <bgfx/bgfx.h>
#include <bimg/bimg.h>

KAZE_NAMESPACE_BEGIN

auto texture::fromImage(ImageHandle image, Bool freeImage) -> TextureHandle
{
    bgfx::TextureHandle texture;
    try {
        const bgfx::Memory *memory;
        if (freeImage)
        {
            memory = bgfx::makeRef(image.data(), image.size(), [](void *ptr, void *userdata) {
                const auto image = static_cast<bimg::ImageContainer *>(userdata);
                bimg::imageFree(image);
            }, image.handle);
        }
        else
        {
            memory = bgfx::makeRef(image.data(), image.size());
        }

        texture = bgfx::createTexture2D(
            image.width(),
            image.height(),
            image.mipCount() > 1,
            image.layerCount(),
            bgfx::TextureFormat::RGBA8,
            BGFX_SAMPLER_POINT,
            memory
        );
    }
    catch(const std::exception &e)
    {
        KAZE_CORE_ERRCODE(Error::StdExcept, "Exception thrown from bgfx::createTexture2D: {}",
            e.what());
        return {};
    }
    catch(...)
    {
        KAZE_CORE_ERRCODE(Error::Unknown, "Unknown error thrown from bgfx::createTexture2D");
        return {};
    }

    if ( !bgfx::isValid(texture) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "Failed to create bgfx texture2d");
        return {};
    }

    return TextureHandle{ texture.idx };
}

auto texture::fromPixels(MemView<void> data, Vec2<Uint> dimensions,
    PixelFormat::Enum srcFormat) -> TextureHandle
{
    if (data.size() == 0) // memory must have substance: no empty textures
    {
        KAZE_CORE_ERRCODE(Error::InvalidArgErr, "Memory passed was null");
        return {};
    }

    const auto srcStride = PixelFormat::getStride(srcFormat);
    if (srcStride == 0)
    {
        KAZE_CORE_ERRCODE(Error::InvalidEnum, "Invalid PixelFormat::Enum");
        return {};
    }

    const auto pixelCount = data.size() / srcStride;
    if (pixelCount != dimensions.x * dimensions.y)
    {
        KAZE_CORE_ERRCODE(Error::InvalidArgErr, "calculated invalid image size to pixel count: "
            "{} * {} != {}", dimensions.x, dimensions.y, pixelCount);
        return {};
    }


    const auto destMemSize = pixelCount * 4;
    const auto dest = memory::alloc(destMemSize);

    if ( !PixelFormat::toRGBA8(static_cast<Ubyte *>(dest), static_cast<const Ubyte *>(data.data()),
        pixelCount, srcFormat) )
    {
        memory::free(dest);
        return {};
    }

    const auto texture = bgfx::createTexture2D(
        static_cast<uint16_t>(dimensions.x),
        static_cast<uint16_t>(dimensions.y),
        false,
        1,
        bgfx::TextureFormat::RGBA8,
        BGFX_SAMPLER_POINT,
        bgfx::makeRef(dest, destMemSize,
            [](void *ptr, void *userptr) {
                memory::free(userptr);
            }, dest)
        );

    if ( !bgfx::isValid(texture) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bgfx::createTexture2D failed");
        memory::free(dest);
        return {};
    }

    return TextureHandle{ texture.idx };
}

auto texture::free(TextureHandle texture) noexcept -> void
{
    if (texture::isValid(texture))
    {
        bgfx::destroy(bgfx::TextureHandle{.idx=texture.handle});
    }
}

auto texture::isValid(TextureHandle texture) noexcept -> Bool
{
    return texture.handle != bgfx::kInvalidHandle;
}

KAZE_NAMESPACE_END
