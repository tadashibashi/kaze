/// @file Texture2D.cpp
/// Implementation for the Texture2D class
#include "Texture2D.h"
#include <kaze/debug.h>
#include <kaze/io/io.h>

#include <bimg/bimg.h>
#include <bgfx/bgfx.h>
#include <bx/error.h>

KAZE_NAMESPACE_BEGIN

static auto loadImage(const void *data, Size byteLength, bimg::ImageContainer *image) -> Bool
{
    if ( !image )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required argument `image` was null");
        return KAZE_FALSE;
    }

    bx::Error error{};
    if ( !bimg::imageParse(*image, data, byteLength, &error) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bimg failed to parse image: {}",
            error.getMessage().getCPtr());
        return KAZE_FALSE;
    }

    return KAZE_TRUE;
}

Texture2D::Texture2D() : m_textureId(bgfx::kInvalidHandle)
{ }

Texture2D::Texture2D(Cstring path) : m_textureId(bgfx::kInvalidHandle)
{
    load(path);
}

Texture2D::~Texture2D()
{
    release();
}

Texture2D::Texture2D(Texture2D &&other) noexcept : m_textureId(other.m_textureId)
{
    other.m_textureId = bgfx::kInvalidHandle;
}

auto Texture2D::operator= (Texture2D &&other) noexcept -> Texture2D &
{
    if (this == &other) return *this;

    release();
    m_textureId = other.m_textureId;
    other.m_textureId = bgfx::kInvalidHandle;

    return *this;
}

auto Texture2D::load(const void *data, Size byteLength) -> Bool
{
    bimg::ImageContainer image;
    if ( !loadImage(data, byteLength, &image) )
    {
        return KAZE_FALSE;
    }

    const auto texture = bgfx::createTexture2D(
        image.m_width,
        image.m_height,
        image.m_numMips > 0,
        image.m_numLayers,
        static_cast<bgfx::TextureFormat::Enum>(image.m_format),
        BGFX_SAMPLER_POINT,
        bgfx::copy(image.m_data, image.m_size)
    );

    if ( !bgfx::isValid(texture) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "Failed to create bgfx texture2d");
        return KAZE_FALSE;
    }

    m_textureId = texture.idx;
    return KAZE_TRUE;
}

auto Texture2D::load(Cstring filepath) -> Bool
{
    Ubyte *data;
    Size byteLength;
    if ( !loadFile(filepath, &data, &byteLength) )
    {
        kaze::release(data);
        return KAZE_FALSE;
    }

    return load(data, byteLength);
}

auto Texture2D::isLoaded() const noexcept -> Bool
{
    return m_textureId != bgfx::kInvalidHandle;
}

auto Texture2D::release() -> void
{
    if (isLoaded())
    {
        bgfx::destroy(bgfx::TextureHandle{.idx=m_textureId});
        m_textureId = bgfx::kInvalidHandle;
    }
}

KAZE_NAMESPACE_END
