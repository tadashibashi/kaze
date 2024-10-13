#include "UniformMgr.h"
#include "Color.h"
#include "Texture2D.h"

#include <kaze/debug.h>

#include <bgfx/bgfx.h>


KAZE_NAMESPACE_BEGIN

const Uint16 Uniform::InvalidHandle = bgfx::kInvalidHandle;

auto Uniform::operator=(const Color &value) -> Uniform &
{
    float fColor[4] = {value.r/255.f, value.g/255.f, value.b/255.f, value.a/255.f};
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

auto Uniform::operator=(const Mat4f &value) -> Uniform & {
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

struct UniformMgr::Impl {

    Impl() : m_uniforms(), m_texUniforms(), m_texUniformsInit(false) {}
    ~Impl() {
        clear();
    }

    auto getTextureUniform(Int slot) -> bgfx::UniformHandle
    {
        if ( !m_texUniformsInit )
        {
            initTextureUniforms();
        }

        return m_texUniforms[slot];
    }

    auto clear() -> void
    {
        for (auto &[key, uniform] : m_uniforms)
        {
            bgfx::destroy(uniform);
        }

        if (m_texUniformsInit)
        {
            for (Int i = 0; i < MaxTextureSlots; ++i)
            {
                bgfx::destroy(m_texUniforms[i]);
            }
            m_texUniformsInit = false;
        }

        m_uniforms.clear();
    }

    Dictionary<String, bgfx::UniformHandle> m_uniforms{};

private:
    auto initTextureUniforms() -> void
    {
        for (Int i = 0; i < MaxTextureSlots; ++i)
        {
            String name = "s_texture" + std::to_string(i);
            const auto handle = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler);
            if ( !bgfx::isValid(handle) )
            {
                throw std::runtime_error("Failed to create texture uniform");
            }

            m_texUniforms[i] = handle;
        }

        m_texUniformsInit = KAZE_TRUE;
    }

    Array<bgfx::UniformHandle, UniformMgr::MaxTextureSlots> m_texUniforms {};
    bool m_texUniformsInit{};
};

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

UniformMgr::UniformMgr() : m(new Impl)
{
}

UniformMgr::~UniformMgr()
{
    delete m;
}

auto UniformMgr::clear() -> void
{
    m->clear();
}

auto UniformMgr::empty() const noexcept -> Bool
{
    return m->m_uniforms.empty();
}

auto UniformMgr::size() const noexcept -> Size
{
    return m->m_uniforms.size();
}

auto UniformMgr::create(StringView name, UniformType type) -> Uniform
{
    bgfx::UniformType::Enum bgfxType;
    switch (type)
    {
    case UniformType::Vec4: bgfxType = bgfx::UniformType::Vec4; break;
    case UniformType::Mat4: bgfxType = bgfx::UniformType::Mat4; break;
    case UniformType::Mat3: bgfxType = bgfx::UniformType::Mat3; break;
    default:
        KAZE_CORE_ERRCODE(Error::InvalidEnum, "Invlaid UniformType passed");
        return Uniform{};
    }

    const auto handle = bgfx::createUniform(name.data(), bgfxType);
    if ( !bgfx::isValid(handle) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "Failed to create uniform");
        return Uniform{};
    }

    m->m_uniforms[name.data()] = handle;

    return Uniform{handle.idx};
}

auto UniformMgr::getUniform(const StringView name) const -> Uniform
{
    const auto it = m->m_uniforms.find(name.data());
    if (it != m->m_uniforms.end())
    {
        return Uniform{it->second.idx};
    }

    KAZE_CORE_ERRCODE(Error::MissingKeyErr, "No uniform with key: \"{}\"", name.data());
    return Uniform{};
}

auto UniformMgr::setTexture(Int slot, const Texture2D &texture) const -> void
{
    KAZE_ASSERT(slot >= 0 && slot < MaxTextureSlots);

    bgfx::setTexture(
        slot,
        m->getTextureUniform(slot),
        bgfx::TextureHandle{.idx=texture.id()});
}

KAZE_NAMESPACE_END
