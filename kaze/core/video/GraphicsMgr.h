#pragma once
#include "Color.h"

#include <kaze/core/lib.h>
#include <kaze/core/video/WindowConstants.h>

#include "UniformMgr.h"

KAZE_NS_BEGIN
class Window;
class VertexLayout;

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
    auto reset(Int width, Int height, WindowInit::Flags flags = WindowInit::None) -> void;

    auto touch(Int viewId) -> void;

    auto frame() -> void;
    auto renderFrame() -> void;

    auto window() const noexcept -> const Window &;
    auto uniforms() const noexcept -> const UniformMgr &;
    auto uniforms() -> UniformMgr &;

    auto getAvailTransientVBuffer(Uint requestedCount, const VertexLayout &layout) const noexcept -> Uint;
    auto getAvailTransientIBuffer(Uint requestedCount) const noexcept -> Uint;
private:
    struct Impl;
    Impl *m;
};

KAZE_NS_END
