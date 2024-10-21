#pragma once
#ifndef kaze_core_video_texturehandle_h_
#define kaze_core_video_texturehandle_h_

#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN

struct TextureHandle {
    TextureHandle();
    explicit TextureHandle(Uint16 handle) : handle(handle) { }

    Uint16 handle;
};

KAZE_NAMESPACE_END

#endif // kaze_core_video_texturehandle_h_

