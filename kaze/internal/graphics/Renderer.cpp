#include "Renderer.h"

#include <kaze/internal/graphics/VertexLayout.h>

#include <kaze/internal/core/debug.h>
#include <kaze/internal/core/math/mathf.h>
#include <kaze/internal/core/platform/backend/backend.h>
#include <kaze/internal/core/platform/backend/window.h>
#include <kaze/internal/core/platform/defines.h>
#include <kaze/internal/core/Window.h>

#include <bgfx/bgfx.h>
#include <bgfx/defines.h>
#include <bgfx/platform.h>

#include <mutex>

KGFX_NS_BEGIN

struct Renderer::Impl {
    Impl() { }
    ~Impl() = default;

    static bool wasInit;
    static std::mutex initLock;

    UniformMgr uniforms{};
    Window window{};
    Size maxTransientVBufferSize{};
    Size maxTransientIBufferSize{};
};

bool Renderer::Impl::wasInit = false;
std::mutex Renderer::Impl::initLock{};

Renderer::Renderer() : m(new Impl())
{

}

Renderer::~Renderer()
{
    close();
    delete m;
}

static auto toBgfxNativeWindowType(
    backend::window::NativePlatformData::WindowType type) -> bgfx::NativeWindowHandleType::Enum
{
    using WindowType = backend::window::NativePlatformData::WindowType;
    switch(type)
    {
        case WindowType::Default: return bgfx::NativeWindowHandleType::Default;
        case WindowType::Wayland: return bgfx::NativeWindowHandleType::Wayland;
        default:
            return bgfx::NativeWindowHandleType::Count;
    }
}

auto Renderer::init(const RendererInit &initConfig) -> Bool
{
    auto window = initConfig.window;

    std::lock_guard lockGuard(Renderer::Impl::initLock);
    if (Renderer::Impl::wasInit)
    {
        KAZE_PUSH_ERR(Error::GR_InitErr, "bgfx was already initialized");
        return False;
    }
    if ( !window )
    {
        KAZE_PUSH_ERR(Error::NullArgErr, "`window` was null");
        return False;
    }

    int width, height;
    if ( !backend::window::getFramebufferSize(window, &width, &height) )
    {
        return False;
    }

    float scaleX, scaleY;
    uint16_t viewWidth, viewHeight;

#if KAZE_PLATFORM_LINUX
    if (backend::window::getContentScale(window, &scaleX, &scaleY))
    {
        viewWidth = static_cast<uint16_t>(width * scaleX);
        viewHeight = static_cast<uint16_t>(height * scaleY);
    }
    else
#endif
    {
        viewWidth = static_cast<uint16_t>(width);
        viewHeight = static_cast<uint16_t>(height);
    }

    const auto platformData = backend::window::getNativeInfo(window);
    bgfx::Init config{};
#if KAZE_PLATFORM_LINUX  // Vulkan crashes when resizing window, it may be a vulkan consideration and not just linux...
    config.type = bgfx::RendererType::OpenGL;
#elif KAZE_PLATFORM_ANDROID
    config.type = bgfx::RendererType::OpenGLES;
#endif
    config.platformData.ndt = platformData.displayType;
    config.platformData.nwh = platformData.windowHandle;
    config.platformData.type = toBgfxNativeWindowType(platformData.type);
    config.resolution.width = viewWidth;
    config.resolution.height = viewHeight;
    config.resolution.reset = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI;
    config.limits.transientIbSize = initConfig.maxTransientIBufferSize;
    config.limits.transientVbSize = initConfig.maxTransientVBufferSize;

#if !KAZE_PLATFORM_EMSCRIPTEN
    bgfx::renderFrame(); // necessary for single-threaded render
#endif
    if ( !bgfx::init(config) )
    {
        KAZE_PUSH_ERR(Error::GR_InitErr, "bgfx failed to init");
        return False;
    }

    bgfx::setViewRect(0, 0, 0, viewWidth, viewHeight);
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, initConfig.clearColor.toRGBA8(), 1.f);
    bgfx::touch(0);
    bgfx::frame();

    Impl::wasInit = true;
    m->window = std::move(Window::fromHandleRef(window));
    m->maxTransientVBufferSize = initConfig.maxTransientVBufferSize;
    m->maxTransientIBufferSize = initConfig.maxTransientIBufferSize;

    mathf::setHomogenousNDC(bgfx::getCaps()->homogeneousDepth);
    return KAZE_TRUE;
}

auto Renderer::wasInit() const noexcept -> Bool
{
    return Impl::wasInit;
}

auto Renderer::close() -> void
{
    std::lock_guard lockGuard(Impl::initLock);
    m->uniforms.clear();
    if (Impl::wasInit)
    {
        bgfx::shutdown();
        Impl::wasInit = false;
    }
}

auto Renderer::setClearColor(const Color &color) -> void
{
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, color.toRGBA8(), 1.f);
}

auto Renderer::reset(Int width, Int height, WindowInit::Flags flags) -> void
{
    Uint bgfxFlags = BGFX_RESET_VSYNC | BGFX_RESET_HIDPI;

    if (flags & WindowInit::Fullscreen)
        bgfxFlags |= BGFX_RESET_FULLSCREEN;
    if (flags & WindowInit::Transparent)
        bgfxFlags |= BGFX_RESET_TRANSPARENT_BACKBUFFER;

    bgfx::reset(width, height, bgfxFlags);
}

auto Renderer::touch(Int viewId) -> void
{
    bgfx::touch(viewId);
}

auto Renderer::frame() -> void
{
    bgfx::frame();
}

auto Renderer::renderFrame() -> void
{
#if !KAZE_PLATFORM_EMSCRIPTEN
    bgfx::renderFrame();
#endif
}

auto Renderer::window() const noexcept -> const Window &
{
    return m->window;
}

auto Renderer::uniforms() const noexcept -> const UniformMgr &
{
    return m->uniforms;
}

auto Renderer::uniforms() -> UniformMgr &
{
    return m->uniforms;
}

auto Renderer::getAvailTransientVBuffer(const Uint requestedCount, const VertexLayout &layout) const noexcept -> Uint
{
    return bgfx::getAvailTransientVertexBuffer(requestedCount, layout.getLayout());
}

auto Renderer::getAvailTransientIBuffer(const Uint requestedCount) const noexcept -> Uint
{
    return bgfx::getAvailTransientIndexBuffer(requestedCount);
}

KGFX_NS_END
