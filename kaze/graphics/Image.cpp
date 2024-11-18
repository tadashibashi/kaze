#include "Image.h"

#include <kaze/core/debug.h>
#include <kaze/core/io/io.h>
#include <kaze/graphics/private/image.h>

KAZE_NS_BEGIN

#define ASSERT_LOADED() KAZE_ASSERT(m_image.isValid(), "Image must be loaded to call this function");

Image::Image() noexcept : m_image() {}

Image::~Image() noexcept
{
    unload();
}

Image::Image(const Image &other) noexcept : Image()
{
    if (other.m_image)
    {
        m_image = image::copy(other.m_image);
    }
}

auto Image::operator=(const Image &other) noexcept -> Image &
{
    if (this == &other) return *this;

    unload();
    if (other.m_image)
    {
        m_image = image::copy(other.m_image);
    }
    else
    {
        m_image = {};
    }

    return *this;
}

Image::Image(Image &&other) noexcept : m_image(other.m_image)
{
    other.m_image = {};
}

auto Image::operator=(Image &&other) noexcept -> Image &
{
    if (this == &other) return *this;

    unload();
    m_image = other.m_image;
    other.m_image = {};

    return *this;
}

auto Image::isLoaded() const noexcept -> Bool
{
    return m_image.isValid();
}

auto Image::load(StringView filepath) -> Bool
{
    Ubyte *data; Size size;
    if ( !file::load(filepath, &data, &size) )
    {
        return KAZE_FALSE;
    }

    const auto result = load(MemView<void>(data, size));
    memory::free(data);
    return result;
}

auto Image::load(MemView<void> mem) -> Bool
{
    const auto image = image::load(mem.data(), mem.size());
    if ( !image.isValid() )
    {
        return KAZE_FALSE;
    }

    unload();
    m_image = image;

    return KAZE_TRUE;
}

auto Image::loadPixels(MemView<void> pixels, Size width,
                         Size height, PixelFormat::Enum srcFormat) -> Bool
{
    const auto image = image::create(width, height, pixels.data(), srcFormat);
    if ( !image.isValid() )
        return KAZE_FALSE;

    unload();
    m_image = image;
    return KAZE_TRUE;
}

auto Image::unload() -> void
{
    if (isLoaded())
    {
        image::free(m_image);
        m_image = {};
    }
}

auto Image::dimensions() const noexcept -> Vec2<Uint>
{
    return { m_image.width(), m_image.height() };
}

auto Image::pixelCount() const noexcept -> Size
{
    return m_image.pixelCount();
}

auto Image::data() const noexcept -> const Ubyte *
{
    return m_image.data();
}

auto Image::data() noexcept -> Ubyte *
{
    return m_image.data();
}

auto Image::size() const noexcept -> Size
{
    return m_image.size();
}

auto Image::mipCount() const noexcept -> Size
{
    return m_image.mipCount();
}

auto Image::layerCount() const noexcept -> Size
{
    return m_image.layerCount();
}

auto Image::isCubeMap() const noexcept -> Bool
{
    return m_image.isCubeMap();
}

KAZE_NS_END
