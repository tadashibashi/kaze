#pragma once
#include <kaze/tk/lib.h>
#include <kaze/core/math/Rect.h>
#include <kaze/core/math/Vec/Vec2.h>
#include <kaze/core/video/Color.h>
#include <kaze/core/video/ShaderProgram.h>
#include <kaze/core/video/Texture2D.h>
#include <kaze/core/video/UniformMgr.h>

KAZE_NS_BEGIN
class GraphicsMgr;
class Window;
KAZE_NS_END

KAZE_TK_NAMESPACE_BEGIN

/// Common options for drawing textures and primitives with a SpriteBatch.
struct SpriteDrawOpts {
    /// Color of the rectangle
    Color tint   {255, 255, 255, 255};
    /// Scale of the image
    Vec2f scale  {1.f, 1.f};
    /// Anchor point from which to position, rotate and scale. Multiplied by the rectangle size.
    Vec2f anchor {.5f, .5f};
    /// Angle in radians by which to rotate the rect
    Float angle  {0};
    /// Depth sorting order
    Float depth  {0};
};

class SpriteBatch {
public:
    struct BatchConfig
    {
        const Float *viewMtx = nullptr;
        const Float *projMtx = nullptr;
    };

    /// TODO: probably just use a normal shader, don't make a new class.
    /// SpriteBatch should have a createShaderProgram() function that returns a SpriteBatch-flavored shader.
    class SubShader
    {
    public:
        SubShader() = default;
        ~SubShader() = default;

        auto load(StringView fragmentPath) -> Bool;

        [[nodiscard]]
        auto isLoaded() const noexcept -> Bool { return m_program.isLinked(); }

        [[nodiscard]]
        auto getProgram() -> const ShaderProgram & { return m_program; }
    private:
        ShaderProgram m_program;
    };

    explicit SpriteBatch();
    ~SpriteBatch();

    auto init(const GraphicsMgr &graphics) -> Bool;
    auto wasInit() const -> Bool;
    auto release() -> void;

    /// Draw a rectangle
    /// \param[in]  rect      rectangle to draw
    /// \param[in]  opts      drawing options [optional]
    auto drawRect(const Recti &rect, const SpriteDrawOpts &opts = SpriteDrawOpts{}) -> void;

    /// Draw a source rectangle within a texture
    /// \param[in]  texture   texture to draw
    /// \param[in]  source    source rectangle within the texture
    /// \param[in]  position  position to draw the texture
    /// \param[in]  opts      drawing options [optional]
    auto drawTexture(const Texture2D *texture, const Recti &source, const Vec2f &position,
        const SpriteDrawOpts &opts = SpriteDrawOpts{}) -> void;

    /// Draw a texture
    /// \param[in]  texture   texture to draw
    /// \param[in]  position  position to draw at
    /// \param[in]  opts      drawing options [optional]
    auto drawTexture(const Texture2D *texture, const Vec2f &position,
        const SpriteDrawOpts &opts = SpriteDrawOpts{}) -> void
    {
        drawTexture(texture,
            {0, 0, static_cast<Int>(texture->size().x), static_cast<Int>(texture->size().y)},
            position, opts);
    }

    auto begin(const BatchConfig &config) -> void;
    auto end() -> void;

private:
    struct Impl;
    Impl *m;
};

KAZE_TK_NAMESPACE_END
