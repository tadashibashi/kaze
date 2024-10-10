#include "Shader.h"
#include <kaze/debug.h>
#include <bgfx/bgfx.h>

KAZE_NAMESPACE_BEGIN

struct Shader::Impl {
    bgfx::ShaderHandle handle{.idx=bgfx::kInvalidHandle};
};

Shader::Shader() : m(new Impl())
{

}

Shader::Shader(const void *data, Size length) : m(new Impl())
{
    compile(data, length);
}

Shader::~Shader()
{
    release();
}

auto Shader::compile(const void *data, const Size length) -> Bool
{
    const auto handle = bgfx::createShader(bgfx::makeRef(data, length));
    if ( !handle.idx )
    {
        KAZE_CORE_ERRCODE(Error::ShaderCompileErr, "Shader failed to compile. Check the logs for info.");
        return KAZE_FALSE;
    }

    m->handle = handle;
    return KAZE_TRUE;
}

auto Shader::isCompiled() const noexcept -> Bool
{
    return m->handle.idx != bgfx::kInvalidHandle;
}

auto Shader::release() -> void
{
    if (isCompiled())
    {
        bgfx::destroy(m->handle);
        m->handle.idx = bgfx::kInvalidHandle;
    }
}

auto Shader::id() const noexcept -> Uint
{
    return static_cast<Uint>(m->handle.idx);
}

KAZE_NAMESPACE_END
