#pragma once

#ifndef kaze_app_spritebatch_h_
#define kaze_app_spritebatch_h_

#include <kaze/kaze.h>
#include <kaze/math/Rect.h>
#include <kaze/math/Vec/Vec2.h>
#include <kaze/video/Color.h>

KAZE_NAMESPACE_BEGIN

class SpriteBatch {
public:
    SpriteBatch();
    ~SpriteBatch();

    /// Draw a rectangle
    auto drawRect(
        Recti rect,
        Color tint = Color::White,
        Vec2f scale = {1.f, 1.f},
        Vec2f anchor = {0, 0},
        Float angle = 0,
        Float depth = 0
    ) -> void;

private:
    struct Impl;
    Impl *m;
};

KAZE_NAMESPACE_END

#endif // kaze_app_spritebatch_h_
