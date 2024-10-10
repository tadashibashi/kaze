#include "Renderable.h"
#include "ShaderProgram.h"

#include <kaze/debug.h>

#include <bgfx/bgfx.h>

KAZE_NAMESPACE_BEGIN

struct Renderable::Impl {
    Int viewId{};
    ShaderProgram program{};
    bgfx::DynamicVertexBufferHandle vbh{.idx=bgfx::kInvalidHandle};
    bgfx::DynamicIndexBufferHandle ibh{.idx=bgfx::kInvalidHandle};
    VertexLayout layout{};
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
    if (indexBuffer.idx == bgfx::kInvalidHandle)
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "bgfx::createDynamicIndexBuffer failed");
        bgfx::destroy(vertexBuffer);
        return KAZE_FALSE;
    }

    m->layout = std::move(config.layout);
    m->program = std::move(program);
    m->vbh = vertexBuffer;
    m->ibh = indexBuffer;
    m->viewId = config.viewId;

    return KAZE_TRUE;
}

auto Renderable::release() -> void
{
    if (m->vbh.idx != bgfx::kInvalidHandle)
    {
        bgfx::destroy(m->vbh);
        m->vbh.idx = bgfx::kInvalidHandle;
    }

    if (m->ibh.idx != bgfx::kInvalidHandle)
    {
        bgfx::destroy(m->ibh);
        m->ibh.idx = bgfx::kInvalidHandle;
    }

    m->program.release();
    m->layout = {};
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

auto Renderable::setVertices(void *data, Size byteSize) -> Renderable &
{
    bgfx::update(m->vbh, 0, bgfx::makeRef(data, byteSize));
    return *this;
}

auto Renderable::setIndices(const Uint16 *data, Size elements) -> Renderable &
{
    bgfx::update(m->ibh, 0, bgfx::makeRef(data, elements * sizeof(Uint16)));
    return *this;
}

auto Renderable::submit() -> void
{
    bgfx::setVertexBuffer(m->viewId, m->vbh);
    bgfx::setIndexBuffer(m->ibh);
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
