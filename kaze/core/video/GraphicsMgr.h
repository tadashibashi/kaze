#pragma once
#ifndef kaze_core_video_graphicsmgr_h_
#define kaze_core_video_graphicsmgr_h_

#include "Color.h"

#include <kaze/core/lib.h>
#include <kaze/core/video/WindowConstants.h>

#include "UniformMgr.h"

KAZE_NAMESPACE_BEGIN
    class Window;

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
    auto wasInit() const noexcept -> Bool;
    auto close() -> void;

    /// Reset graphics setting and backbuffer size
    auto reset(Int width, Int height) -> void;

    auto frame() -> void;
    auto renderFrame() -> void;

    auto window() const noexcept -> const Window &;
    auto uniforms() const noexcept -> const UniformMgr &;
    auto uniforms() -> UniformMgr &;
private:
    struct Impl;
    Impl *m;
};

#endif // kaze_core_video_graphicsmgr_h_

KAZE_NAMESPACE_END
