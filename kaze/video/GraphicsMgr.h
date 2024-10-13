#pragma once
#ifndef kaze_video_graphicsmgr_h_
#define kaze_video_graphicsmgr_h_

#include "Color.h"

#include <kaze/kaze.h>
#include <kaze/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN

/// Sets up graphics API functionality
class GraphicsMgr {
public:
    GraphicsMgr();
    ~GraphicsMgr();

    auto init(WindowHandle window, Color clearColor = Color(100, 154, 206, 235)) -> Bool;
    auto close() -> void;

    /// Reset graphics setting and backbuffer size
    auto reset(Int width, Int height) -> void;

    auto frame() -> void;
    auto renderFrame() -> void;
private:
    struct Impl;
    Impl *m;
};

#endif // kaze_video_graphicsmgr_h_

KAZE_NAMESPACE_END
