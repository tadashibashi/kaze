/// \file Window.cpp
/// Contains implementation of the Window class
#include "Window.h"
#include <kaze/internal/core/platform/backend/backend.h>
#include <kaze/internal/core/debug.h>

KAZE_NS_BEGIN

Window::Window() noexcept : m_window(), m_ownsWindow()
{ }

Window::~Window() noexcept
{
    if (m_ownsWindow)
        close();
}

auto Window::fromHandleRef(WindowHandle window) noexcept -> Window
{
    Window w;
    w.m_window = window;
    w.m_ownsWindow = false;

    return w;
}

Window::Window(Window &&other) noexcept :
    m_window(other.m_window), m_ownsWindow(other.m_ownsWindow)
{
    other.m_ownsWindow = false;
    other.m_window = nullptr;
}

auto Window::operator=(Window &&other) noexcept -> Window &
{
    m_window = other.m_window;
    m_ownsWindow = other.m_ownsWindow;

    other.m_window = nullptr;
    other.m_ownsWindow = false;

    return *this;
}

auto Window::open(const Cstring title, const Size width, const Size height,
    const WindowInit::Flags initFlags) noexcept -> Bool
{
    WindowHandle window;
    if ( !backend::window::open(title, width, height, initFlags, &window))
        return false;

    if (m_ownsWindow)
        close();
    m_window = window;
    m_ownsWindow = true;
    return KAZE_TRUE;
}

auto Window::isOpen() const noexcept -> Bool
{
    if ( !m_window )
        return KAZE_FALSE;

    bool open;
    if ( !backend::window::isOpen(m_window, &open) )
        return KAZE_FALSE;
    return open;
}

auto Window::ownsWindow() const noexcept -> Bool
{
    return m_ownsWindow;
}

auto Window::close() noexcept -> Bool
{
    if ( !m_ownsWindow )
    {
        KAZE_PUSH_ERR(Error::UnownedPointerCleanup, "Attempted to close non-owned window");
        return KAZE_FALSE;
    }

    if (m_window)
    {
        const auto result = backend::window::close(m_window);
        m_window = nullptr;

        return result;
    }

    return KAZE_TRUE;
}

auto Window::setTitle(const Cstring title) noexcept -> Window &
{
    backend::window::setTitle(m_window, title);
    return *this;
}

auto Window::setTitle(const String &title) noexcept -> Window &
{
    backend::window::setTitle(m_window, title.c_str());
    return *this;
}

auto Window::getTitle() const noexcept -> Cstring
{
    Cstring title;
    if ( !backend::window::getTitle(m_window, &title) || !title )
        return "";
    return title;
}

auto Window::getDisplaySize() const noexcept -> Vec2i
{
    Vec2i size;
    backend::window::getFramebufferSize(m_window, &size.x, &size.y);
    return size;
}

auto Window::setSize(Vec2i size) noexcept -> Window &
{
    backend::window::setSize(m_window, size.x, size.y);
    return *this;
}

auto Window::setSize(const Int width, const Int height) noexcept -> Window &
{
    backend::window::setSize(m_window, width, height);
    return *this;
}

auto Window::getSize() const noexcept -> Vec2i
{
    Vec2i size;
    backend::window::getSize(m_window, &size.x, &size.y);
    return size;
}

auto Window::getDPIScale() const noexcept -> Float
{
    int width, virtWidth;
    backend::window::getFramebufferSize(m_window, &width, nullptr);
    backend::window::getSize(m_window, &virtWidth, nullptr);

    return static_cast<Float>(width) / static_cast<Float>(virtWidth);
}

auto Window::isBordered() const noexcept -> Bool
{
    bool bordered;
    if ( !backend::window::isBordered(m_window, &bordered) )
        return KAZE_FALSE;
    return bordered;
}

auto Window::setBordered(const Bool value) noexcept -> Window &
{
    backend::window::setBordered(m_window, value);
    return *this;
}

auto Window::isFullscreen() const noexcept -> Bool
{
    bool fullscreen;
    if ( !backend::window::isFullscreen(m_window, &fullscreen) )
        return KAZE_FALSE;
    return fullscreen;
}

auto Window::setFullscreen(const Bool value) noexcept -> Window &
{
    if (isFullscreen() == value) return *this;

    backend::window::setFullscreen(m_window, value);
    return *this;
}

auto Window::getFullscreenMode() const noexcept -> FullscreenMode
{
    FullscreenMode mode;
    if ( !backend::window::getFullscreenMode(m_window, &mode) )
        return FullscreenMode::Unknown;
    return mode;
}

auto Window::setFullscreenMode(const FullscreenMode mode) noexcept -> Window &
{
    backend::window::setFullscreenMode(m_window, mode);
    return *this;
}

auto Window::getPosition() const noexcept -> Vec2i
{
    Vec2i result;
    backend::window::getPosition(m_window, &result.x, &result.y);

    return result;
}

auto Window::setPosition(Vec2i position) noexcept -> Window &
{
    backend::window::setPosition(m_window, position.x, position.y);
    return *this;
}

auto Window::getDisplayRect() const noexcept -> Recti
{
    Recti result;
    backend::window::getPosition(m_window, &result.x, &result.y);
    backend::window::getFramebufferSize(m_window, &result.w, &result.h);

    return result;
}

auto Window::maximize() noexcept -> Window &
{
    backend::window::maximize(m_window);
    return *this;
}

auto Window::isMaximized() const noexcept -> Bool
{
    bool maximized;
    if ( !backend::window::isMaximized(m_window, &maximized) )
        return false;
    return maximized;
}

auto Window::minimize() noexcept -> Window &
{
    backend::window::minimize(m_window);
    return *this;
}

auto Window::isMinimized() const noexcept -> Bool
{
    bool minimized;
    if ( !backend::window::isMinimized(m_window, &minimized) )
        return KAZE_FALSE;
    return minimized;
}

auto Window::restore() noexcept -> Window &
{
    backend::window::restore(m_window);
    return *this;
}

auto Window::setHidden(const Bool value) noexcept -> Window &
{
    backend::window::setHidden(m_window, value);
    return *this;
}

auto Window::isHidden() const noexcept -> Bool
{
    bool hidden;
    if (!backend::window::isHidden(m_window, &hidden) )
        return false;
    return hidden;
}

auto Window::setFloating(const Bool value) noexcept -> Window &
{
    backend::window::setFloating(m_window, value);
    return *this;
}

auto Window::isFloating() const noexcept -> Bool
{
    bool floating;
    if ( !backend::window::isFloating(m_window, &floating) )
        return KAZE_FALSE;
    return floating;
}

auto Window::setTransparent(const Bool value) noexcept -> Window &
{
    backend::window::setTransparent(m_window, value);
    return *this;
}

auto Window::isTransparent() const noexcept -> Bool
{
    bool transparent;
    if ( !backend::window::isTransparent(m_window, &transparent) )
        return KAZE_FALSE;
    return transparent;
}

auto Window::focus() noexcept -> Window &
{
    backend::window::focus(m_window);
    return *this;
}

auto Window::isFocused() const noexcept -> Bool
{
    bool focused;
    if ( !backend::window::isFocused(m_window, &focused) )
        return KAZE_FALSE;
    return focused;
}

auto Window::setCursorMode(CursorMode mode) noexcept -> Window &
{
    backend::window::setCursorMode(m_window, mode);
    return *this;
}

auto Window::getCursorMode() const noexcept -> CursorMode
{
    auto mode = CursorMode::Count;
    backend::window::getCursorMode(m_window, &mode);

    return mode;
}

auto Window::getHandle() const -> ConstWindowHandle
{
    return m_window;
}

auto Window::getHandle() -> WindowHandle
{
    return m_window;
}

KAZE_NS_END
