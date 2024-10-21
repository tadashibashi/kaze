#include "Shader.h"
#include <kaze/core/debug.h>
#include <bgfx/bgfx.h>
#include <kaze/core/io/FileBuffer.h>

#include <filesystem>

KAZE_NAMESPACE_BEGIN

struct Shader::Impl {
    bgfx::ShaderHandle handle{.idx=bgfx::kInvalidHandle};
};

Shader::Shader() : m(new Impl())
{

}

Shader::Shader(const Memory mem) : Shader()
{
    compile(mem);
}

Shader::Shader(const StringView filepath) : Shader()
{
    compile(filepath);
}

Shader::Shader(Shader &&other) noexcept : m(other.m)
{
    other.m = nullptr;
}

Shader &Shader::operator=(Shader &&other) noexcept
{
    if (this == &other) return *this;

    if (m != nullptr)
    {
        release();
        delete m;
    }
    m = other.m;
    other.m = nullptr;

    return *this;
}

Shader::~Shader()
{
    if (m)
    {
        release();
        delete m;
    }
}

String Shader::makePath(const StringView folder, const StringView shaderName)
{
    Cstring subFolder;
    switch(bgfx::getRendererType())
    {
    case bgfx::RendererType::OpenGLES:   subFolder = "essl"; break;
    case bgfx::RendererType::OpenGL:     subFolder = "glsl"; break;
    case bgfx::RendererType::Vulkan:     subFolder = "spv"; break;
    case bgfx::RendererType::Metal:      subFolder = "mtl"; break;
    case bgfx::RendererType::Direct3D11: subFolder = "dx11"; break;
    case bgfx::RendererType::Direct3D12: subFolder = "dx12"; break;
    default:
        KAZE_CORE_ERRCODE(Error::Unsupported, "shader language not supported for renderer type: {}",
            bgfx::getRendererName(bgfx::getRendererType()));
        return {};
    }

    return (std::filesystem::path(folder) / subFolder / shaderName).string();
}

auto Shader::compile(const Memory mem) -> Bool
{
    const auto handle = bgfx::createShader( bgfx::makeRef(mem.data(), mem.size()) );
    if ( !bgfx::isValid(handle) )
    {
        KAZE_CORE_ERRCODE(Error::ShaderCompileErr, "Shader failed to compile. Check the logs for info.");
        return KAZE_FALSE;
    }

    m->handle = handle;
    return KAZE_TRUE;
}

auto Shader::compile(const StringView path) -> Bool
{

    if (const FileBuffer file(path); file.isOpen())
    {
        const auto handle = bgfx::createShader( bgfx::copy(file.data(), file.size()) );
        if ( !bgfx::isValid(handle) )
        {
            KAZE_CORE_ERRCODE(Error::ShaderCompileErr, "Shader failed to compile. Check the logs for info.");
            return KAZE_FALSE;
        }

        m->handle = handle;
        return KAZE_TRUE;
    }

    return KAZE_FALSE;
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
