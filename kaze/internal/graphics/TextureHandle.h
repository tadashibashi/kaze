#pragma once
#include <kaze/internal/graphics/lib.h>

KGFX_NS_BEGIN

struct TextureHandle {
    TextureHandle();
    explicit TextureHandle(Uint16 handle) : handle(handle) { }

    Uint16 handle;
};

KGFX_NS_END

