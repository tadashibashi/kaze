#include "GraphicsMgr.h"

#include <kaze/debug.h>
#include <kaze/platform/backend.h>

#include <bgfx/bgfx.h>
#include <bgfx/platform.h>
#include <mutex>

KAZE_NAMESPACE_BEGIN

struct GraphicsMgr::Impl {
    Impl() { }
    ~Impl() = default;

    static bool wasInit;
    static std::mutex initLock;
};
bool GraphicsMgr::Impl::wasInit = false;
std::mutex GraphicsMgr::Impl::initLock{};

GraphicsMgr::GraphicsMgr() : m(new Impl())
{
}

GraphicsMgr::~GraphicsMgr()
{
    close();
    delete m;
}

auto GraphicsMgr::init(const GraphicsInit &initConfig) -> Bool
{
    auto window = initConfig.window;

    std::lock_guard lockGuard(Impl::initLock);
    if (Impl::wasInit)
    {
        KAZE_CORE_ERRCODE(Error::GR_InitErr, "bgfx was already initialized");
        return KAZE_FALSE;
    }
    if ( !window )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "`window` was null");
        return KAZE_FALSE;
    }

    int width, height;
    if ( !backend::window::getFramebufferSize(window, &width, &height) )
    {
        return KAZE_FALSE;
    }

    const auto platformData = backend::window::getNativeInfo(window);
    bgfx::Init config{};
    config.platformData.ndt = platformData.displayType;
    config.platformData.nwh = platformData.windowHandle;
    config.resolution.width = width;
    config.resolution.height = height;
    config.resolution.reset = BGFX_RESET_VSYNC;
    config.limits.transientIbSize = initConfig.maxTransientIBufferSize;
    config.limits.transientVbSize = initConfig.maxTransientVBufferSize;

    bgfx::renderFrame(); // necessary for single-threaded render

    if ( !bgfx::init(config) )
    {
        KAZE_CORE_ERRCODE(Error::GR_InitErr, "bgfx failed to init");
        return KAZE_FALSE;
    }

    bgfx::setViewRect(0, 0, 0, static_cast<uint16_t>(width), static_cast<uint16_t>(height));
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, initConfig.clearColor.toRGBA8(), 1.f);
    bgfx::touch(0);
    bgfx::frame();

    Impl::wasInit = true;
    return KAZE_TRUE;
}

auto GraphicsMgr::close() -> void
{
    std::lock_guard lockGuard(Impl::initLock);
    if (Impl::wasInit)
    {
        bgfx::shutdown();
        Impl::wasInit = false;
    }
}

auto GraphicsMgr::reset(Int width, Int height) -> void
{
    bgfx::reset(width, height, BGFX_RESET_VSYNC); // todo: expose flags?
}

auto GraphicsMgr::frame() -> void
{
    bgfx::frame();
}

auto GraphicsMgr::renderFrame() -> void
{
    bgfx::renderFrame();
}

KAZE_NAMESPACE_END
