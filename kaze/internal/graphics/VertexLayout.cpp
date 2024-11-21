#include "VertexLayout.h"

#include <bgfx/bgfx.h>

KGFX_NS_BEGIN

struct VertexLayout::Impl {
    bgfx::VertexLayout layout{};
};

VertexLayout::VertexLayout() : m(new Impl()) { }
VertexLayout::~VertexLayout() { delete m; }

VertexLayout::VertexLayout(const VertexLayout &other) : m(new Impl())
{
    m->layout = other.m->layout;
}

auto VertexLayout::operator=(const VertexLayout &other) -> VertexLayout &
{
    m->layout = other.m->layout;
    return *this;
}

VertexLayout::VertexLayout(VertexLayout &&other) noexcept : m(other.m)
{
    other.m = nullptr;
}

auto VertexLayout::operator=(VertexLayout &&other) noexcept -> VertexLayout &
{
    if (this == &other) return *this;

    delete m;
    m = other.m;
    other.m = nullptr;

    return *this;
}

auto VertexLayout::begin() -> VertexLayout &
{
    m->layout.begin();
    return *this;
}

auto VertexLayout::end() -> VertexLayout &
{
    m->layout.end();
    return *this;
}

auto VertexLayout::add(VertexAttrib::Enum attrib, Uint8 count, VertexAttribType::Enum type, Bool normalized, Bool asInt) -> VertexLayout &
{
    m->layout.add(static_cast<bgfx::Attrib::Enum>(attrib),
        count, static_cast<bgfx::AttribType::Enum>(type), normalized, asInt);
    return *this;
}

auto VertexLayout::skip(Uint8 bytes) -> VertexLayout &
{
    m->layout.skip(bytes);
    return *this;
}

auto VertexLayout::getLayout() const noexcept -> const bgfx::VertexLayout &
{
    return m->layout;
}

auto VertexLayout::has(VertexAttrib::Enum attrib) const -> Bool
{
    return m->layout.has(static_cast<bgfx::Attrib::Enum>(attrib));
}

auto VertexLayout::getOffset(VertexAttrib::Enum attrib) const -> Uint16
{
    return m->layout.getOffset(static_cast<bgfx::Attrib::Enum>(attrib));
}

auto VertexLayout::getStride() const -> Uint16
{
    return m->layout.getStride();
}

KGFX_NS_END
