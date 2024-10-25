/// \file Texture2D.cpp
/// Implementation for the Texture2D class
#include "Texture2D.h"

#include <kaze/core/debug.h>
#include <kaze/core/io/io.h>
#include <kaze/core/video/Image.h>
#include <kaze/core/video/private/image.h>
#include <kaze/core/video/private/texture.h>

#include <bimg/decode.h>
#include <bgfx/bgfx.h>


KAZE_NAMESPACE_BEGIN

Texture2D::Texture2D() : m_texture()
{ }

Texture2D::Texture2D(const Cstring path) : Texture2D()
{
    load(path);
}

Texture2D::~Texture2D()
{
    release();
}

Texture2D::Texture2D(Texture2D &&other) noexcept : m_texture(other.m_texture), m_size(other.m_size)
{
    other.m_texture = {};
    other.m_size = {};
}

auto Texture2D::operator= (Texture2D &&other) noexcept -> Texture2D &
{
    if (this == &other) return *this;

    release();
    m_texture = other.m_texture;
    m_size = other.m_size;
    other.m_texture = {};
    other.m_size = {};

    return *this;
}

auto Texture2D::load(const void *data, const Size size) -> Bool
{
    const auto image = image::load(data, size);
    if ( !image )
    {
        return KAZE_FALSE;
    }

    const auto imageSize = Vec2<Uint>{ image.width(), image.height() };

    auto texture = texture::fromImage(image, true);
    if ( !texture::isValid(texture) )
    {
        image::free(image);
        return KAZE_FALSE;
    }

    release();
    m_texture = texture;
    m_size = imageSize;
    return KAZE_TRUE;
}

auto Texture2D::loadPixels(MemView<Color> pixels, Vec2<Uint> dimensions) -> Bool
{
    const auto texture = texture::fromPixels(pixels, dimensions, PixelFormat::RGBA8);
    if ( !texture::isValid(texture) )
    {
        return KAZE_FALSE;
    }

    // Done, clean up and commit changes
    release();
    m_texture = texture;
    m_size = dimensions;
    return KAZE_TRUE;
}

auto Texture2D::loadPixels(MemView<void> data, Vec2<Uint> dimensions, PixelFormat::Enum srcFormat) -> Bool
{
    const auto texture = texture::fromPixels(data, dimensions, srcFormat);
    if ( !texture::isValid(texture) )
    {
        return KAZE_FALSE;
    }

    // Done, clean up and commit changes
    release();
    m_texture = texture;
    m_size = dimensions;
    return KAZE_TRUE;
}

auto Texture2D::load(const Cstring path) -> Bool
{
    Ubyte *data;
    Size size;
    if ( !file::load(path, &data, &size) )
    {
        return KAZE_FALSE;
    }

    const auto result = load(data, size);
    memory::free(data);
    return result;
}

auto Texture2D::loadImage(const Image &image) -> Bool
{
    return loadPixels(MemView<void>(image.data(), image.size()),
        image.dimensions(), PixelFormat::RGBA8);
}

auto Texture2D::isLoaded() const noexcept -> Bool
{
    return texture::isValid(m_texture);
}

auto Texture2D::release() -> void
{
    if (texture::isValid(m_texture))
    {
        texture::free(m_texture);
        m_texture = TextureHandle();
    }
}

KAZE_NAMESPACE_END
