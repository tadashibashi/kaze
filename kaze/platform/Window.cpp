#include "Window.h"
#include "PlatformBackend.h"

KAZE_NAMESPACE_BEGIN

Window::Window() noexcept : m_window()
{

}

Window::~Window() noexcept
{
    close();
}

Bool Window::open(const Cstring title, const Size width, const Size height, const WindowInit::Flags initFlags) noexcept
{
    backend::WindowHandle window;
    if ( !backend::window::open(title, width, height, initFlags, &window))
        return false;

    close();
    m_window = window;

    return KAZE_TRUE;
}

Bool Window::isOpen() const noexcept
{
    if ( !m_window )
        return KAZE_FALSE;

    bool open;
    if ( !backend::window::isOpen(m_window, &open) )
        return KAZE_FALSE;
    return open;
}

Bool Window::close() noexcept
{
    if (m_window)
    {
        const auto result = backend::window::close(m_window);
        m_window = nullptr;

        return result;
    }

    return KAZE_FALSE;
}

Window &Window::setTitle(const Cstring title) noexcept
{
    backend::window::setTitle(m_window, title);
    return *this;
}

Window& Window::setTitle(const String &title) noexcept
{
    backend::window::setTitle(m_window, title.c_str());
    return *this;
}

Cstring Window::getTitle() const noexcept
{
    Cstring title;
    if ( !backend::window::getTitle(m_window, &title) || !title )
        return "";
    return title;
}

Vec2i Window::getDisplaySize() const noexcept
{
    Vec2i size;
    backend::window::getFramebufferSize(m_window, &size.x, &size.y);
    return size;
}

Window &Window::setSize(Vec2i size) noexcept
{
    backend::window::setSize(m_window, size.x, size.y);
    return *this;
}

Window &Window::setSize(const Int width, const Int height) noexcept
{
    backend::window::setSize(m_window, width, height);
    return *this;
}

Vec2i Window::getSize() const noexcept
{
    Vec2i size;
    backend::window::getSize(m_window, &size.x, &size.y);
    return size;
}

Float Window::getDPIScale() const noexcept
{
    int width, virtWidth;
    backend::window::getFramebufferSize(m_window, &width, nullptr);
    backend::window::getSize(m_window, &virtWidth, nullptr);

    return static_cast<Float>(width) / static_cast<Float>(virtWidth);
}

Bool Window::isBordered() const noexcept
{
    bool bordered;
    if ( !backend::window::isBordered(m_window, &bordered) )
        return KAZE_FALSE;
    return bordered;
}

Window &Window::setBordered(const Bool value) noexcept
{
    backend::window::setBordered(m_window, value);
    return *this;
}

Bool Window::isFullscreen() const noexcept
{
    bool fullscreen;
    if ( !backend::window::isFullscreen(m_window, &fullscreen) )
        return KAZE_FALSE;
    return fullscreen;
}

Window &Window::setFullscreen(const Bool value) noexcept
{
    if (isFullscreen() == value) return *this;

    backend::window::setFullscreen(m_window, value);
    return *this;
}

FullscreenMode Window::getFullscreenMode() const noexcept
{
    FullscreenMode mode;
    if ( !backend::window::getFullscreenMode(m_window, &mode) )
        return FullscreenMode::Unknown;
    return mode;
}

Window & Window::setFullscreenMode(FullscreenMode mode) noexcept
{
    backend::window::setFullscreenMode(m_window, mode);
    return *this;
}

Vec2i Window::getPosition() const noexcept
{
    Vec2i result;
    backend::window::getPosition(m_window, &result.x, &result.y);

    return result;
}

Recti Window::getDisplayRect() const noexcept
{
    Recti result;
    backend::window::getPosition(m_window, &result.x, &result.y);
    backend::window::getFramebufferSize(m_window, &result.w, &result.h);

    return result;
}

Window &Window::maximize() noexcept
{
    backend::window::maximize(m_window);
    return *this;
}

Bool Window::isMaximized() const noexcept
{
    bool maximized;
    if ( !backend::window::isMaximized(m_window, &maximized) )
        return false;
    return maximized;
}

Window &Window::minimize() noexcept
{
    backend::window::minimize(m_window);
    return *this;
}

Bool Window::isMinimized() const noexcept
{
    bool minimized;
    if ( !backend::window::isMinimized(m_window, &minimized) )
        return KAZE_FALSE;
    return minimized;
}

Window &Window::restore() noexcept
{
    backend::window::restore(m_window);
    return *this;
}

Window &Window::setHidden(const Bool value) noexcept
{
    backend::window::setHidden(m_window, value);
    return *this;
}

Bool Window::isHidden() const noexcept
{
    bool hidden;
    if (!backend::window::isHidden(m_window, &hidden) )
        return false;
    return hidden;
}

Window& Window::setFloating(const Bool value) noexcept
{
    backend::window::setFloating(m_window, value);
    return *this;
}

Bool Window::isFloating() const noexcept
{
    bool floating;
    if ( !backend::window::isFloating(m_window, &floating) )
        return KAZE_FALSE;
    return floating;
}

Window &Window::setTransparent(const Bool value) noexcept
{
    backend::window::setTransparent(m_window, value);
    return *this;
}

Bool Window::isTransparent() const noexcept
{
    bool transparent;
    if ( !backend::window::isTransparent(m_window, &transparent) )
        return KAZE_FALSE;
    return transparent;
}

Window &Window::focus() noexcept
{
    backend::window::focus(m_window);
    return *this;
}

Bool Window::isFocused() const noexcept
{
    bool focused;
    if ( !backend::window::isFocused(m_window, &focused) )
        return KAZE_FALSE;
    return focused;
}


KAZE_NAMESPACE_END
