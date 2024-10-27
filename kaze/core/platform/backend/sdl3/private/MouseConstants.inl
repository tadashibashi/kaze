#pragma once
#include <kaze/core/input/MouseConstants.h>
#include <kaze/core/lib.h>
#include <SDL3/SDL_mouse.h>

KAZE_NAMESPACE_BEGIN

static constexpr Array<Uint8, static_cast<Int>(MouseBtn::Count)> s_toSDLMouseButton = {
    SDL_BUTTON_LEFT,
    SDL_BUTTON_RIGHT,
    SDL_BUTTON_MIDDLE,
    SDL_BUTTON_X1,
    SDL_BUTTON_X2,
    0, ///< these buttons are not supported by SDL3
    0,
    0,
};

static Array<MouseBtn, static_cast<Int>(SDL_BUTTON_X2)> s_toMouseButton{};

KAZE_NAMESPACE_END
