#include "ShaderProgram.h"
#include <kaze/internal/core/debug.h>

#include <bgfx/bgfx.h>

KGFX_NS_BEGIN

struct ShaderProgram::Impl {
    Impl() : program{.idx=bgfx::kInvalidHandle} {}
    ~Impl() = default;
    bgfx::ProgramHandle program;
};

ShaderProgram::ShaderProgram() : m(new Impl()) { }
ShaderProgram::~ShaderProgram()
{
    if (m)
    {
        release();
        delete m;
    }

}

ShaderProgram::ShaderProgram(ShaderProgram &&other) noexcept : m(other.m)
{
    other.m = nullptr;
}

auto ShaderProgram::operator=(ShaderProgram &&other) noexcept -> ShaderProgram &
{
    if (this == &other) return *this;

    if (m)
    {
        release();
        delete m;
    }

    m = other.m;
    other.m = nullptr;
    return *this;
}

auto ShaderProgram::link(const Shader &vertShader, const Shader &fragShader) -> Bool
{
    if (!vertShader.isCompiled() || !fragShader.isCompiled())
        return KAZE_FALSE;

    bgfx::ShaderHandle vert, frag;
    vert.idx = static_cast<Uint16>(vertShader.id());
    frag.idx = static_cast<Uint16>(fragShader.id());

    const auto program = bgfx::createProgram(vert, frag);
    if ( program.idx == bgfx::kInvalidHandle)
    {
        KAZE_PUSH_ERR(Error::ShaderLinkErr, "Failed to link shader program");
        return KAZE_FALSE;
    }


    release();
    m->program = program;
    return KAZE_TRUE;
}

auto ShaderProgram::isLinked() const noexcept -> Bool
{
    return m->program.idx != bgfx::kInvalidHandle;
}

auto ShaderProgram::release() -> void
{
    if (isLinked())
    {
        bgfx::destroy(m->program);
        m->program.idx = bgfx::kInvalidHandle;
    }
}

auto ShaderProgram::id() const noexcept -> Uint
{
    return m->program.idx;
}

auto ShaderProgram::submit(Int viewId) -> void
{
    KAZE_ASSERT(isLinked());
    bgfx::submit(viewId, m->program);
}

KGFX_NS_END
