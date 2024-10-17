#pragma once

#ifndef kaze_app_spritebatch_h_
#define kaze_app_spritebatch_h_

#include <kaze/kaze.h>
#include <kaze/math/Rect.h>
#include <kaze/math/Vec/Vec2.h>
#include <kaze/video/Color.h>
#include <kaze/video/ShaderProgram.h>
#include <kaze/video/Texture2D.h>
#include <kaze/video/UniformMgr.h>

KAZE_NAMESPACE_BEGIN

class Window;

/// Options for drawing textures and primitives
struct DrawOpts {
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

    class Shader
    {
    public:
        Shader() = default;
        ~Shader() = default;

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

    auto init(const Window &window, const UniformMgr &uniforms) -> Bool;
    auto wasInit() const -> Bool;
    auto release() -> void;

    /// Draw a rectangle
    /// @param[in]  rect   rectangle to draw
    /// @param[in]  opts   drawing options [optional]
    auto drawRect(const Recti &rect, const DrawOpts &opts = DrawOpts{}) -> void;

    /// Draw a source rectangle within a texture
    /// @param[in]  texture   texture to draw
    /// @param[in]  source    source rectangle within the texture
    /// @param[in]  position  position to draw the texture
    /// @param[in]  opts      drawing options [optional]
    auto drawTexture(const Texture2D &texture, const Recti &source, const Vec2f &position, const DrawOpts &opts = DrawOpts{}) -> void;

    /// Draw a texture
    /// @param[in]  texture   texture to draw
    /// @param[in]  position  position to draw at
    /// @param[in]  opts      drawing options [optional]
    auto drawTexture(const Texture2D &texture, const Vec2f &position, const DrawOpts &opts = DrawOpts{}) -> void
    {
        drawTexture(texture, {0, 0, texture.size().x, texture.size().y}, position, opts);
    }

    auto begin(const BatchConfig &config) -> void;
    auto end() -> void;

private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_app_spritebatch_h_
