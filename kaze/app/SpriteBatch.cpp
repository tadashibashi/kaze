#include "SpriteBatch.h"

#include <kaze/debug.h>
#include <kaze/math/Vec/Vec3.h>
#include <kaze/video/Renderable.h>
#include <kaze/video/Window.h>


KAZE_NAMESPACE_BEGIN

auto SpriteBatch::Shader::load(StringView fragmentPath) -> Bool
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
        Uint16 textureId;
    };

    struct Batch
    {
        Uint offset;      ///< starting index of quad
        Uint count;       ///< number of quads in the batch
        Uint16 textureId; ///< gpu id of texture
    };

    explicit Impl() : m_window(), m_uniforms()
    {

    }

    ~Impl()
    {

    }

    auto init(const Window &window, const UniformMgr &uniforms) -> Bool
    {
        if (wasInit())
            return KAZE_TRUE;

        const auto result = m_renderable.init({
            .viewId = 0,
            .vertShader = std::move(kaze::Shader(kaze::Shader::makePath("kaze/shaders", "spritebatch_v.sc.bin"))),
            .fragShader = std::move(kaze::Shader(kaze::Shader::makePath("kaze/shaders", "spritebatch_f.sc.bin"))),
            .layout = VertexLayout()
                .begin()
                    .add(Attrib::Position, 3, AttribType::Float)
                    .add(Attrib::TexCoord0, 2, AttribType::Float)
                    .add(Attrib::Color0, 4, AttribType::Uint8, KAZE_TRUE)
                .end(),
            .initialIndexCount = 60000,
            .initialVertexCount = 40000,
        });

        if (result)
        {
            m_window = &window;
            m_uniforms = &uniforms;
        }

        return result;
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
    }

    auto wasInit() const -> Bool
    {
        return m_renderable.wasInit();
    }

    auto begin(const BatchConfig &config) -> void
    {
        KAZE_ASSERT( !m_batchStarted, "Mismatched `SpriteBatch::begin` call. Did you forget to call `SpriteBatch::end`?");

        m_quads.clear();

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
            const auto size = m_window->getSize();
            m_defaultProjMtx = Mat4f::fromOrtho(0, size.x, 0, size.y);
            m_projMtx = m_defaultProjMtx.data();
        }

        m_batchStarted = KAZE_TRUE;
    }

    auto drawRect(const Recti &rect, const DrawOpts &opts) -> void
    {
        KAZE_ASSERT(false, "TODO: need to implement this!");
    }

    auto drawTexture(const Texture2D &texture, const Recti &source, const Vec2f &position, const DrawOpts &opts) -> void
    {
        KAZE_ASSERT(texture.isLoaded());

        const auto texCoords = Rectf(
            static_cast<Float>(source.x) / static_cast<Float>(texture.size().x),
            static_cast<Float>(source.y) / static_cast<Float>(texture.size().y),
            static_cast<Float>(source.w) / static_cast<Float>(texture.size().x),
            static_cast<Float>(source.h) / static_cast<Float>(texture.size().y)
        );

        // Calculate anchor point in pixels
        const auto anchor = opts.anchor * texture.size() * opts.scale;

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
        quad.textureId = texture.id();

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
        if (m_quads.empty()) return;

        m_vertices.clear();
        m_batches.clear();

        // add any necessary indices
        if (const auto targetQuads = m_quads.size(), targetIndices = targetQuads * 6; targetIndices > m_indices.size())
        {
            m_indexUpdatedLastSize = m_indices.size();
            m_indices.reserve(targetIndices);
            for (auto curQuad = m_indices.size() / 6; curQuad < targetQuads; ++curQuad)
            {
                auto vertexIndex = curQuad * 4;
                m_indices.emplace_back(vertexIndex);
                m_indices.emplace_back(vertexIndex + 1);
                m_indices.emplace_back(vertexIndex + 2);
                m_indices.emplace_back(vertexIndex + 2);
                m_indices.emplace_back(vertexIndex + 3);
                m_indices.emplace_back(vertexIndex);
            }
        }

        // create the batches
        for (Uint curQuad = 0, lastTextureId = bgfx::kInvalidHandle;
            const auto &[topleft, bottomleft, topright, bottomright, textureId] : m_quads)
        {
            if (textureId != lastTextureId)
            {
                m_batches.emplace_back(Batch{
                    .offset = curQuad,
                    .count = 1,
                    .textureId = textureId
                });

                lastTextureId = textureId;
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
    }

    auto renderBatches() -> void
    {
        const auto size = m_window->getDisplaySize();
        m_renderable.setViewRect({0, 0, size.x, size.y});
        m_renderable.setViewTransform(m_viewMtx, m_projMtx);
        m_renderable.setVertices(m_vertices);

        if (m_indexUpdatedLastSize >= 0)
        {
            m_renderable.setIndices(m_indices.data(), m_indices.size());
            m_indexUpdatedLastSize = -1;
        }

        for (const auto &[offset, count, textureId] : m_batches)
        {
            m_uniforms->setTexture(0, textureId); // TODO: move setTexture to renderable? (needs to contain ref to UniformMgr)
            m_renderable.activateIndices(offset * 6, count * 6);
            m_renderable.activateVertices(offset * 4, count * 4);
            m_renderable.submit();
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
    const Window *m_window{};
    const UniformMgr *m_uniforms{};

    Int64 m_indexUpdatedLastSize{-1};
};

SpriteBatch::SpriteBatch() : m(new Impl())
{

}

SpriteBatch::~SpriteBatch()
{
    delete m;
}

auto SpriteBatch::init(const Window &window, const UniformMgr &uniforms) -> Bool
{
    return m->init(window, uniforms);
}

auto SpriteBatch::wasInit() const -> Bool
{
    return m->wasInit();
}

auto SpriteBatch::release() -> void
{
    m->release();
}

auto SpriteBatch::drawRect(const Recti &rect, const DrawOpts &opts) -> void
{
    m->drawRect(rect, opts);
}

auto SpriteBatch::drawTexture(const Texture2D &texture, const Recti &source, const Vec2f &position, const DrawOpts &opts) -> void
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

KAZE_NAMESPACE_END
