#include "Uniform.h"
#include "Color.h"

#include <kaze/core/debug.h>
#include <bgfx/bgfx.h>

KGFX_NS_BEGIN

const Uint16 Uniform::InvalidHandle = bgfx::kInvalidHandle;

auto Uniform::operator=(const Color &value) -> Uniform &
{
    const float fColor[4] = {value.r/255.f, value.g/255.f, value.b/255.f, value.a/255.f};
    bgfx::setUniform({.idx=m_handle}, fColor);
    return *this;
}

auto Uniform::operator=(const Vec4f &value) -> Uniform &
{
    bgfx::setUniform({.idx=m_handle}, &value);
    return *this;
}

auto Uniform::operator=(const Mat3f &value) -> Uniform &
{
    bgfx::setUniform({.idx=m_handle}, &value);
    return *this;
}

auto Uniform::operator=(const Mat4f &value) -> Uniform &
{
    bgfx::setUniform({.idx=m_handle}, &value);
    return *this;
}

auto Uniform::setColor(const Color &value) -> Uniform &
{
    return *this = value;
}

auto Uniform::setVec4(const Vec4f &value) -> Uniform &
{
    return *this = value;
}

auto Uniform::setMat4(const Mat4f &value) -> Uniform &
{
    return *this = value;
}

auto Uniform::setMat4(const Float *array) -> Uniform &
{
    bgfx::setUniform({.idx=m_handle}, array);
    return *this;
}

auto Uniform::setMat3(const Mat3f &value) -> Uniform &
{
    return *this = value;
}

auto Uniform::setMat3(const Float *array) -> Uniform &
{
    bgfx::setUniform({.idx=m_handle}, array);
    return *this;
}

auto Uniform::type() const noexcept -> UniformType
{
    bgfx::UniformInfo info;
    bgfx::getUniformInfo({.idx=m_handle}, info);
    switch(info.type)
    {
    case bgfx::UniformType::Mat3: return UniformType::Mat3;
    case bgfx::UniformType::Mat4: return UniformType::Mat4;
    case bgfx::UniformType::Vec4: return UniformType::Vec4;
    default: return UniformType::Unknown;
    }
}

auto Uniform::name() const noexcept -> String
{
    bgfx::UniformInfo info;
    bgfx::getUniformInfo({.idx=m_handle}, info);
    return info.name;
}

KAZE_NS_END
