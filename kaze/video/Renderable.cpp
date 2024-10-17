#include "Renderable.h"
#include "ShaderProgram.h"

#include <kaze/debug.h>

#include <bgfx/bgfx.h>
#include <kaze/memory.h>

KAZE_NAMESPACE_BEGIN
    struct Renderable::Impl {
    Int viewId{};
    ShaderProgram program{};
    bgfx::DynamicVertexBufferHandle vbh{.idx=bgfx::kInvalidHandle};
    bgfx::DynamicIndexBufferHandle ibh{.idx=bgfx::kInvalidHandle};
    VertexLayout layout{};

    Memory vertexData{}, indexData{};
};

Renderable::Renderable() : m(new Impl()) {}
Renderable::~Renderable() { release(); delete m; }

auto Renderable::init(const Init &config) -> Bool
{
    ShaderProgram program;
    if ( !program.link(config.vertShader, config.fragShader) )
        return KAZE_FALSE;

    const auto vertexBuffer = bgfx::createDynamicVertexBuffer(config.initialVertexCount,
        config.layout.getLayout(), BGFX_BUFFER_ALLOW_RESIZE);
    if (vertexBuffer.idx == bgfx::kInvalidHandle)
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bgfx::createDynamicVertexBuffer failed");
        return KAZE_FALSE;
    }

    const auto indexBuffer = bgfx::createDynamicIndexBuffer(config.initialIndexCount,
        BGFX_BUFFER_ALLOW_RESIZE);
    if ( !bgfx::isValid(indexBuffer) )
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bgfx::createDynamicIndexBuffer failed");
        bgfx::destroy(vertexBuffer);
        return KAZE_FALSE;
    }

    release();
    m->layout = std::move(config.layout);
    m->program = std::move(program);
    m->vbh = vertexBuffer;
    m->ibh = indexBuffer;
    m->viewId = config.viewId;

    return KAZE_TRUE;
}

auto Renderable::wasInit() const -> Bool
{
    return m->program.isLinked() && bgfx::isValid(m->ibh) && bgfx::isValid(m->vbh);
}

auto Renderable::release() -> void
{
    if ( bgfx::isValid(m->vbh) )
    {
        bgfx::destroy(m->vbh);
        m->vbh.idx = bgfx::kInvalidHandle;
    }

    if ( bgfx::isValid(m->ibh) )
    {
        bgfx::destroy(m->ibh);
        m->ibh.idx = bgfx::kInvalidHandle;
    }

    m->program.release();
    m->layout = {};
}

auto Renderable::activateVertices(Uint startVertex, Uint vertexCount) -> Renderable &
{
    bgfx::setVertexBuffer(0, m->vbh, startVertex, vertexCount);
    return *this;
}

auto Renderable::activateIndices(Uint startIndex, Uint indexCount) -> Renderable &
{
    bgfx::setIndexBuffer(m->ibh, startIndex, indexCount);
    return *this;
}

auto Renderable::setViewTransform(const Mat4f &view, const Mat4f &projection) -> Renderable &
{
    bgfx::setViewTransform(m->viewId, view.data(), projection.data());
    return *this;
}

auto Renderable::setViewTransform(const Float *view, const Float *projection) -> Renderable &
{
    bgfx::setViewTransform(m->viewId, view, projection);
    return *this;
}

auto Renderable::setViewRect(const Recti &rect) -> Renderable &
{
    bgfx::setViewRect(m->viewId, rect.x, rect.y, rect.w, rect.h);
    return *this;
}

auto Renderable::setVertices(const Memory mem) -> Renderable &
{
    if (mem.byteLength() > 0)
    {
        m->vertexData = mem;
    }
    return *this;
}

auto Renderable::setIndices(const Uint16 *data, const Size elements) -> Renderable &
{
    m->indexData = Memory(data, elements * sizeof(Uint16));
    return *this;
}

auto Renderable::submit() -> void
{
    if (m->vertexData.byteLength() > 0)
    {
        bgfx::TransientVertexBuffer tvb;
        const auto stride = m->layout.getStride();
        bgfx::allocTransientVertexBuffer(&tvb, m->vertexData.byteLength() / stride, m->layout.getLayout());
        copy(tvb.data, m->vertexData.data(), m->vertexData.byteLength());
        bgfx::setVertexBuffer(0, &tvb);
    }

    if (m->indexData.byteLength() > 0)
    {
        bgfx::TransientIndexBuffer tib;
        bgfx::allocTransientIndexBuffer(&tib, m->indexData.byteLength() / sizeof(Uint16));
        copy(tib.data, m->indexData.data(), m->indexData.byteLength());
        bgfx::setIndexBuffer(&tib);
    }

    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A); // todo: expose these options later?
    m->program.submit(m->viewId);
}

auto Renderable::submit(const Uint vertexStart, const Uint vertexCount, const Uint indexStart, const Uint indexCount) const -> void
{
    bgfx::TransientVertexBuffer tvb;
    bgfx::allocTransientVertexBuffer(&tvb, vertexCount, m->layout.getLayout());
    const auto stride = m->layout.getStride();
    copy(tvb.data, (Ubyte *)m->vertexData.data() + stride * vertexStart, stride * vertexCount);
    bgfx::setVertexBuffer(0, &tvb);

    bgfx::TransientIndexBuffer tib;
    bgfx::allocTransientIndexBuffer(&tib, indexCount);
    copy(tib.data, (Ubyte *)m->indexData.data() + sizeof(Uint16) * indexStart, sizeof(Uint16) * indexCount);
    bgfx::setIndexBuffer(m->ibh, indexStart, indexCount);
    bgfx::setState(BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A); // todo: expose these options later?
    m->program.submit(m->viewId);
}

auto Renderable::setViewId(Int viewId) -> Renderable &
{
    m->viewId = viewId;
    return *this;
}

auto Renderable::getViewId() const noexcept -> Int
{
    return m->viewId;
}

KAZE_NAMESPACE_END
