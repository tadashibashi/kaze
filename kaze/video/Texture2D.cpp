/// @file Texture2D.cpp
/// Implementation for the Texture2D class
#include "Texture2D.h"
#include <kaze/debug.h>
#include <kaze/io/io.h>

#include <bimg/decode.h>
#include <bgfx/bgfx.h>
#include <bx/error.h>

KAZE_NAMESPACE_BEGIN

static bx::DefaultAllocator defaultAlloc{};

/// Load image from file data
/// @param[in]  data   data buffer
/// @param[in]  size   number of bytes in data buffer
/// @returns image data, make sure to call bimg::freeImage on it.
static auto loadImage(const void *data, const Size size) -> bimg::ImageContainer *
{
    if ( !data )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required argument `data` was null");
        return nullptr;
    }

    bx::Error error{};
    bimg::ImageContainer *result = nullptr;
    if ( result = bimg::imageParse(&defaultAlloc, data, size, bimg::TextureFormat::RGBA8, &error); result == nullptr )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bimg failed to parse image: {}",
            error.getMessage().getCPtr());
        return nullptr;
    }

    return result;
}

Texture2D::Texture2D() : m_textureId(bgfx::kInvalidHandle)
{ }

Texture2D::Texture2D(const Cstring path) : m_textureId(bgfx::kInvalidHandle)
{
    load(path);
}

Texture2D::~Texture2D()
{
    release();
}

Texture2D::Texture2D(Texture2D &&other) noexcept : m_textureId(other.m_textureId), m_size(other.m_size)
{
    other.m_textureId = bgfx::kInvalidHandle;
    other.m_size = {};
}

auto Texture2D::operator= (Texture2D &&other) noexcept -> Texture2D &
{
    if (this == &other) return *this;

    release();
    m_textureId = other.m_textureId;
    m_size = other.m_size;
    other.m_textureId = bgfx::kInvalidHandle;
    other.m_size = {};

    return *this;
}

auto Texture2D::load(const void *data, const Size size) -> Bool
{
    bimg::ImageContainer *image = loadImage(data, size);
    if ( image == nullptr )
    {
        return KAZE_FALSE;
    }

    bgfx::TextureHandle texture;
    try {
        texture = bgfx::createTexture2D(
            image->m_width,
            image->m_height,
            image->m_numMips > 1,
            image->m_numLayers,
            static_cast<bgfx::TextureFormat::Enum>(image->m_format),
            BGFX_SAMPLER_POINT,
            bgfx::makeRef(image->m_data, image->m_size)
        );
    }
    catch(const std::exception &e)
    {
        KAZE_CORE_ERRCODE(Error::StdExcept, "Exception thrown from bgfx::createTexture2D: {}",
            e.what());
        bimg::imageFree(image);
        return KAZE_FALSE;
    }
    catch(...)
    {
        KAZE_CORE_ERRCODE(Error::Unknown, "Unknown error thrown from bgfx::createTexture2D");
        bimg::imageFree(image);
        return KAZE_FALSE;
    }

    const auto tempSize = Vec2i(image->m_width, image->m_height);

    bimg::imageFree(image);

    if ( !bgfx::isValid(texture) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "Failed to create bgfx texture2d");
        return KAZE_FALSE;
    }

    release();
    m_textureId = texture.idx;
    m_size = tempSize;
    return KAZE_TRUE;
}

auto Texture2D::loadPixels(Mem<Color> pixels, Size width, Size height) -> Bool
{
    if (pixels.size() != width * height)
    {
        KAZE_CORE_ERRCODE(Error::LogicErr, "`width * height` must equal `pixelCount`, instead got: `{} * {} != {}`",
            width, height, pixels.size());
        return KAZE_FALSE;
    }

    bgfx::TextureHandle texture;
    try {
        texture = bgfx::createTexture2D(width, height, false, 1, bgfx::TextureFormat::RGBA8,
            BGFX_TEXTURE_NONE | BGFX_SAMPLER_POINT, bgfx::makeRef(pixels.data(), sizeof(Color) * pixels.size()));
    }
    catch(const std::exception &e)
    {
        KAZE_CORE_ERRCODE(Error::StdExcept, "Exception thrown from bgfx::createTexture2D: {}",
            e.what());
        return KAZE_FALSE;
    }
    catch(...)
    {
        KAZE_CORE_ERRCODE(Error::Unknown, "Unknown error thrown from bgfx::createTexture2D");
        return KAZE_FALSE;
    }

    if ( !bgfx::isValid(texture) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "Failed to load Texture2D from pixels");
        return KAZE_FALSE;
    }

    release();
    m_textureId = texture.idx;
    m_size = Vec2i(width, height);
    return KAZE_TRUE;
}

auto Texture2D::load(const Cstring path) -> Bool
{
    Ubyte *data;
    Size size;
    if ( !kaze::loadFile(path, &data, &size) )
    {
        kaze::release(data);
        return KAZE_FALSE;
    }

    const auto result = load(data, size);
    kaze::release(data);
    return result;
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
