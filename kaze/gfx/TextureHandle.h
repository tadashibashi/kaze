#pragma once
#include <kaze/gfx/lib.h>

KGFX_NS_BEGIN

struct TextureHandle {
    TextureHandle();
    explicit TextureHandle(Uint16 handle) : handle(handle) { }

    Uint16 handle;
};

KAZE_NS_END

