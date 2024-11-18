#pragma once
#include <kaze/core/lib.h>

KAZE_NS_BEGIN

struct TextureHandle {
    TextureHandle();
    explicit TextureHandle(Uint16 handle) : handle(handle) { }

    Uint16 handle;
};

KAZE_NS_END

