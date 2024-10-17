#pragma once
#ifndef kaze_video_graphicsmgr_h_
#define kaze_video_graphicsmgr_h_

#include "Color.h"

#include <kaze/kaze.h>
#include <kaze/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN

struct GraphicsInit
{
    WindowHandle window;  ///< A valid window that is already open
    Color clearColor = {100, 154, 206, 235}; /// Default background clear color
    Size maxTransientVBufferSize = 4000 * 1024; /// Max transient vertex buffer size (hard limit)
    Size maxTransientIBufferSize = 6000 * 1024; /// Max transient index buffer size (hard limit)
};

/// Sets up graphics API functionality
class GraphicsMgr {
public:
    GraphicsMgr();
    ~GraphicsMgr();

    auto init(const GraphicsInit &initConfig) -> Bool;
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
