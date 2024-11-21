#include "UniformMgr.h"

#include <kaze/internal/core/debug.h>

#include <bgfx/bgfx.h>

KGFX_NS_BEGIN

struct UniformMgr::Impl {

    Impl() : m_uniforms(), m_texUniforms() {
        for (auto i = 0; i < m_texUniforms.size(); ++i)
        {
            m_texUniforms[i] = {.idx = bgfx::kInvalidHandle};
        }
    }
    ~Impl() {
        clear();
    }

    auto getTextureUniform(const Int slot) -> bgfx::UniformHandle
    {
        if ( !bgfx::isValid(m_texUniforms[slot]) )
        {
            String name = "s_texture" + std::to_string(slot);
            const auto handle = bgfx::createUniform(name.c_str(), bgfx::UniformType::Sampler);
            if ( !bgfx::isValid(handle) )
            {
                throw std::runtime_error("Failed to create texture uniform");
            }

            m_texUniforms[slot] = handle;
        }

        return m_texUniforms[slot];
    }

    auto clear() -> void
    {
        for (auto &[key, uniform] : m_uniforms)
        {
            bgfx::destroy(uniform);
        }

        for (Int i = 0; i < m_texUniforms.size(); ++i)
        {
            if (bgfx::isValid(m_texUniforms[i]))
                bgfx::destroy(m_texUniforms[i]);
        }

        m_uniforms.clear();
    }

    [[nodiscard]]
    auto empty() const noexcept { return m_uniforms.empty(); }

    [[nodiscard]]
    auto size() const noexcept { return m_uniforms.size(); }

    auto create(const StringView name, const UniformType type) -> Uniform
    {
        bgfx::UniformType::Enum bgfxType;
        switch (type)
        {
        case UniformType::Vec4: bgfxType = bgfx::UniformType::Vec4; break;
        case UniformType::Mat4: bgfxType = bgfx::UniformType::Mat4; break;
        case UniformType::Mat3: bgfxType = bgfx::UniformType::Mat3; break;
        default:
            KAZE_PUSH_ERR(Error::InvalidEnum, "Invlaid UniformType passed");
            return Uniform{};
        }

        const auto handle = bgfx::createUniform(name.data(), bgfxType);
        if ( !bgfx::isValid(handle) )
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "Failed to create uniform");
            return Uniform{};
        }

        m_uniforms[name.data()] = handle;

        return Uniform{handle.idx};
    }

    [[nodiscard]]
    auto getUniform(const StringView name) const
    {
        const auto it = m_uniforms.find(name.data());
        if (it != m_uniforms.end())
        {
            return Uniform{it->second.idx};
        }

        KAZE_PUSH_ERR(Error::MissingKeyErr, "No uniform with key: \"{}\"", name);
        return Uniform{};
    }

private:
    Dictionary<String, bgfx::UniformHandle> m_uniforms{};
    Array<bgfx::UniformHandle, UniformMgr::MaxTextureSlots> m_texUniforms {};
};

UniformMgr::UniformMgr() : m(new Impl)
{
}

UniformMgr::~UniformMgr()
{
    delete m;
}

UniformMgr::UniformMgr(UniformMgr &&other) noexcept : m(other.m)
{
    other.m = nullptr;
}

auto UniformMgr::operator=(UniformMgr &&other) noexcept -> UniformMgr &
{
    if (this == &other) return *this;

    clear();

    m = other.m;
    other.m = nullptr;

    return *this;
}

UniformMgr::UniformMgr(const std::initializer_list<std::pair<StringView, UniformType>> &uniforms) : UniformMgr()
{
    for (const auto &[name, type] : uniforms)
    {
        create(name, type);
    }
}

auto UniformMgr::operator=(const std::initializer_list<std::pair<StringView, UniformType>> &uniforms) -> UniformMgr &
{
    m->clear();

    for (const auto &[name, type] : uniforms)
    {
        create(name, type);
    }

    return *this;
}

auto UniformMgr::clear() -> void
{
    m->clear();
}

auto UniformMgr::empty() const noexcept -> Bool
{
    return m->empty();
}

auto UniformMgr::size() const noexcept -> Size
{
    return m->size();
}

auto UniformMgr::create(StringView name, UniformType type) -> Uniform
{
    return m->create(name, type);
}

auto UniformMgr::getUniform(const StringView name) const -> Uniform
{
    return m->getUniform(name);
}

auto UniformMgr::setTexture(const Int slot, const Texture2D &texture) const -> void
{
    setTexture(slot, texture.handle());
}

auto UniformMgr::setTexture(const Int slot, TextureHandle texture) const -> void
{
    KAZE_ASSERT(slot >= 0 && slot < MaxTextureSlots);

    bgfx::setTexture(
        slot,
        m->getTextureUniform(slot),
        bgfx::TextureHandle{.idx=texture.handle});
}

auto UniformMgr::operator[](const String &name) const -> Uniform
{
    return getUniform(name);
}

KGFX_NS_END
