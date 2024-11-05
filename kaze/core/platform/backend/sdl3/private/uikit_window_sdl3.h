#pragma once

#include <kaze/core/lib.h>
#include <SDL3/SDL.h>

KAZE_NS_BEGIN

namespace backend {
    /// Init Window for Apple Devices, and returns a metal layer
    auto initMetalUIKitWindow(SDL_Window *window) -> void *;
}

KAZE_NS_END
