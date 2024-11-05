#include "SpriteBatch.h"

#include <kaze/core/debug.h>
#include <kaze/core/math/Vec/Vec3.h>
#include <kaze/core/platform/filesys/filesys.h>
#include <kaze/core/video/GraphicsMgr.h>
#include <kaze/core/video/Renderable.h>
#include <kaze/core/video/Window.h>
#include <filesystem>

KAZE_TK_NAMESPACE_BEGIN

auto SpriteBatch::SubShader::load(StringView fragmentPath) -> Bool
{
    // TODO: Implement this!
    throw std::runtime_error("Not implemented");
}

struct SpriteBatch::Impl
{
    struct Vertex
    {
        Vec3f position;
        Vec2f texcoord;
        Color color;
    };

    struct Quad
    {
        Vertex topleft, bottomleft, topright, bottomright;
        TextureHandle texture;
    };

    struct Batch
    {
        Uint offset;      ///< starting index of quad
        Uint count;       ///< number of quads in the batch
        TextureHandle texture; ///< gpu id of texture
    };

    explicit Impl() : m_graphics()
    {

    }

    ~Impl()
    {

    }

    auto init(const GraphicsMgr &graphics) -> Bool
    {
        if (wasInit())
            return KAZE_TRUE;

        if ( !graphics.wasInit())
        {
            KAZE_PUSH_ERR(Error::RuntimeErr, "GraphicsMgr passed to SpriteBatch::init is required to be "
                "initialized: was not init");
            return KAZE_FALSE;
        }

        if ( !m_pixelTexture.loadPixels(makeRef(&Color::White, 1), {1, 1}) )
            return KAZE_FALSE;

        const std::filesystem::path baseDir = filesys::getBaseDir();

        const auto result = m_renderable.init({
            .viewId = 0,
            .vertShader = std::move(Shader( (baseDir / Shader::makePath("kaze/shaders", "spritebatch_v.sc.bin")).string() )),
            .fragShader = std::move(Shader( (baseDir / Shader::makePath("kaze/shaders", "spritebatch_f.sc.bin")).string() )),
            .layout = VertexLayout()
                .begin()
                    .add(Attrib::Position, 3, AttribType::Float)
                    .add(Attrib::TexCoord0, 2, AttribType::Float)
                    .add(Attrib::Color0, 4, AttribType::Uint8, KAZE_TRUE)
                .end(),
        });

        if ( !result )
            return KAZE_FALSE;

        m_graphics = &graphics;
        return KAZE_TRUE;
    }

    auto release() -> void
    {
        m_renderable.release();
        m_batches.clear();
        m_quads.clear();
        m_indices.clear();
        m_vertices.clear();

        m_viewMtx = nullptr;
        m_batchStarted = KAZE_FALSE;
        m_pixelTexture.release();
    }

    auto wasInit() const -> Bool
    {
        return m_renderable.wasInit();
    }

    auto begin(const BatchConfig &config) -> void
    {
        KAZE_ASSERT( !m_batchStarted, "Mismatched `SpriteBatch::begin` call. Did you forget to call `SpriteBatch::end`?");

        if (config.viewMtx != nullptr)
            m_viewMtx = config.viewMtx;
        else
        {
            m_viewMtx = m_defaultViewMtx.data();
        }

        if (config.projMtx != nullptr)
        {
            m_projMtx = config.projMtx;
        }
        else
        {
            // create transformation matrix based on window size
            const auto size = m_graphics->window().getSize();
            m_defaultProjMtx = Mat4f::fromOrtho(0, size.x, 0, size.y);
            m_projMtx = m_defaultProjMtx.data();
        }

        m_batchStarted = KAZE_TRUE;
    }

    auto drawRect(const Recti &rect, const SpriteDrawOpts &opts) -> void
    {
        drawTexture(&m_pixelTexture, {0, 0, 1, 1}, static_cast<Vec2f>(rect.position), opts);
    }

    auto drawTexture(const Texture2D *texture, const Recti &source, const Vec2f &position, const SpriteDrawOpts &opts) -> void
    {
        KAZE_ASSERT(texture->isLoaded());

        const auto texSize = texture->size();

        const auto texCoords = Rectf(
            static_cast<Float>(source.x) / static_cast<Float>(texSize.x),
            static_cast<Float>(source.y) / static_cast<Float>(texSize.y),
            static_cast<Float>(source.w) / static_cast<Float>(texSize.x),
            static_cast<Float>(source.h) / static_cast<Float>(texSize.y)
        );

        // Calculate anchor point in pixels
        const auto anchor = opts.anchor * texSize * opts.scale;

        const auto dest = Rectf {
            mathf::round(position.x), //< round snaps coords to pixel grid
            mathf::round(position.y),
            mathf::round(source.w * opts.scale.x),
            mathf::round(source.h * opts.scale.y)
        };

        Vec2f offsetTopLeft, offsetBottomLeft, offsetTopRight, offsetBottomRight;
        if (opts.angle != 0) // apply rotation
        {
            mathf::rotateCoords(-anchor.x, -anchor.y, opts.angle, &offsetTopLeft.x, &offsetTopLeft.y);
            mathf::rotateCoords(-anchor.x, dest.h - anchor.y, opts.angle, &offsetBottomLeft.x, &offsetBottomLeft.y);
            mathf::rotateCoords(dest.w - anchor.x, -anchor.y, opts.angle, &offsetTopRight.x, &offsetTopRight.y);
            mathf::rotateCoords(dest.w - anchor.x, dest.h - anchor.y, opts.angle, &offsetBottomRight.x, &offsetBottomRight.y);
        }
        else                 // no rotation
        {
            offsetTopLeft = -anchor;
            offsetBottomLeft = {-anchor.x, dest.h - anchor.y};
            offsetTopRight = {dest.w - anchor.x, -anchor.y};
            offsetBottomRight = {dest.w - anchor.x, dest.h - anchor.y};
        }

        Quad quad;
        quad.texture = texture->handle();

        quad.topleft.color = opts.tint;
        quad.topleft.position = Vec3f(dest.x + offsetTopLeft.x, dest.y + offsetTopLeft.y, opts.depth);
        quad.topleft.texcoord = texCoords.topLeft();

        quad.topright.color = opts.tint;
        quad.topright.position = Vec3f(dest.x + offsetTopRight.x, dest.y + offsetTopRight.y, opts.depth);
        quad.topright.texcoord = texCoords.topRight();

        quad.bottomleft.color = opts.tint;
        quad.bottomleft.position = Vec3f(dest.x + offsetBottomLeft.x, dest.y + offsetBottomLeft.y, opts.depth);
        quad.bottomleft.texcoord = texCoords.bottomLeft();

        quad.bottomright.color = opts.tint;
        quad.bottomright.position = Vec3f(dest.x + offsetBottomRight.x, dest.y + offsetBottomRight.y, opts.depth);
        quad.bottomright.texcoord = texCoords.bottomRight();

        m_quads.emplace_back(quad);
    }

    auto end() -> void
    {
        KAZE_ASSERT(m_batchStarted, "Mismatched `SpriteBatch::end` call. Did you remember to call `SpriteBatch::begin`?");

        createBatches();
        renderBatches();
        m_batchStarted = KAZE_FALSE;
    }

private:
    auto createBatches() -> void
    {
        m_vertices.clear();
        m_batches.clear();

        if (m_quads.empty()) return;

        auto targetQuads = m_quads.size();
        const auto maxQuadsI = m_graphics->getAvailTransientIBuffer(targetQuads * 6) / 6;
        const auto maxQuadsV = m_graphics->getAvailTransientVBuffer(targetQuads * 4, m_renderable.getLayout()) / 4;
        targetQuads = mathf::min(maxQuadsI, maxQuadsV);

        // add any necessary indices
        if (const auto targetIndices = targetQuads * 6;
            targetIndices > m_indices.size())
        {
            m_indexUpdatedLastSize = m_indices.size();
            m_indices.reserve(targetIndices);
            for (auto curQuad = m_indices.size() / 6; curQuad < targetQuads; ++curQuad)
            {
                auto vertexIndex = static_cast<Uint16>(curQuad * 4);
                m_indices.emplace_back(vertexIndex);
                m_indices.emplace_back(vertexIndex + 1);
                m_indices.emplace_back(vertexIndex + 2);
                m_indices.emplace_back(vertexIndex + 2);
                m_indices.emplace_back(vertexIndex + 3);
                m_indices.emplace_back(vertexIndex);
            }
        }

        // create the batches
        auto lastTexture = TextureHandle();
        for (Uint curQuad = 0;
            const auto &[topleft, bottomleft, topright, bottomright, texture] : m_quads)
        {
            if (curQuad >= targetQuads)
                break;
            if (texture.handle != lastTexture.handle)
            {
                m_batches.emplace_back(Batch{
                    .offset = curQuad,
                    .count = 1,
                    .texture = texture
                });

                lastTexture = texture;
            }
            else
            {
                ++m_batches.back().count;
            }

            m_vertices.emplace_back(topleft);
            m_vertices.emplace_back(bottomleft);
            m_vertices.emplace_back(bottomright);
            m_vertices.emplace_back(topright);
            ++curQuad;
        }

        m_quads.clear();
    }

    auto renderBatches() -> void
    {
        if (m_batches.empty())
            return;
        const auto size = m_graphics->window().getDisplaySize();
        m_renderable.setViewRect({0, 0, size.x, size.y});
        m_renderable.setViewTransform(m_viewMtx, m_projMtx);
        m_renderable.setVertices(makeRef(m_vertices.data(), m_vertices.size()));

        if (m_indexUpdatedLastSize >= 0)
        {
            m_renderable.setIndices(m_indices.data(), m_indices.size());
            m_indexUpdatedLastSize = -1;
        }

        for (const auto &[offset, count, texture] : m_batches)
        {
            m_graphics->uniforms().setTexture(0, texture); // TODO: move setTexture to renderable? (needs to contain ref to UniformMgr)
            // m_renderable.activateIndices(offset * 6, count * 6);
            // m_renderable.activateVertices(offset * 4, count * 4);
            m_renderable.submit(offset * 4, count * 4, offset * 6, count * 6);
        }
    }

    List<Uint16> m_indices{};
    List<Vertex> m_vertices{};
    List<Quad> m_quads{};
    List<Batch> m_batches{}; ///< outlines the batches
    const Float *m_viewMtx{};
    const Float *m_projMtx{};
    Mat4f m_defaultViewMtx{};
    Mat4f m_defaultProjMtx{};

    Bool m_batchStarted{};
    Renderable m_renderable{};
    const GraphicsMgr *m_graphics{};

    Int64 m_indexUpdatedLastSize{-1};
    Texture2D m_pixelTexture{};
};

SpriteBatch::SpriteBatch() : m(new Impl())
{

}

SpriteBatch::~SpriteBatch()
{
    delete m;
}

auto SpriteBatch::init(const GraphicsMgr &graphics) -> Bool
{
    return m->init(graphics);
}

auto SpriteBatch::wasInit() const -> Bool
{
    return m->wasInit();
}

auto SpriteBatch::release() -> void
{
    m->release();
}

auto SpriteBatch::drawRect(const Recti &rect, const SpriteDrawOpts &opts) -> void
{
    m->drawRect(rect, opts);
}

auto SpriteBatch::drawTexture(const Texture2D *texture, const Recti &source, const Vec2f &position, const SpriteDrawOpts &opts) -> void
{
    m->drawTexture(texture, source, position, opts);
}

auto SpriteBatch::begin(const BatchConfig &config) -> void
{
    m->begin(config);
}

auto SpriteBatch::end() -> void
{
    m->end();
}

KAZE_TK_NAMESPACE_END
