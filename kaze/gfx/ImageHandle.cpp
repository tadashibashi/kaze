#include "ImageHandle.h"
#include <bimg/bimg.h>

KGFX_NS_BEGIN

#define GET_IMG() static_cast<bimg::ImageContainer *>(handle)
#define ASSERT_IMG() KAZE_ASSERT(handle, "ImageHandle must be loaded")

auto ImageHandle::data() const noexcept -> const Ubyte *
{
    ASSERT_IMG();
    return static_cast<const Ubyte *>(GET_IMG()->m_data);
}

auto ImageHandle::data() noexcept -> Ubyte *
{
    ASSERT_IMG();
    return static_cast<Ubyte *>(GET_IMG()->m_data);
}

auto ImageHandle::width() const noexcept -> Uint
{
    ASSERT_IMG();
    return GET_IMG()->m_width;
}

auto ImageHandle::height() const noexcept -> Uint
{
    ASSERT_IMG();
    return GET_IMG()->m_height;
}

auto ImageHandle::pixelCount() const noexcept -> Size
{
    ASSERT_IMG();
    return GET_IMG()->m_width * GET_IMG()->m_height;
}

auto ImageHandle::size() const noexcept -> Size
{
    ASSERT_IMG();
    return GET_IMG()->m_size;
}

auto ImageHandle::mipCount() const noexcept -> Size
{
    ASSERT_IMG();
    return GET_IMG()->m_numMips;
}

auto ImageHandle::layerCount() const noexcept -> Size
{
    ASSERT_IMG();
    return GET_IMG()->m_numLayers;
}

auto ImageHandle::isCubeMap() const noexcept -> Bool
{
    ASSERT_IMG();
    return GET_IMG()->m_cubeMap;
}

KAZE_NS_END
