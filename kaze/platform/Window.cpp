#include "Window.h"
#include "PlatformBackend.h"

KAZE_NAMESPACE_BEGIN

using backend::PlatformBackend;

Window::Window() : m_window()
{

}

Window::~Window()
{
    close();
}

Bool Window::open(const Cstring title, const Size width, const Size height, const WindowInit::Flags initFlags)
{
    const auto platformWindow = PlatformBackend::windowCreate(title, width, height, initFlags);
    if (!platformWindow)
    {
        return KAZE_FALSE;
    }

    close();
    m_window = platformWindow;

    return KAZE_TRUE;
}

Bool Window::isOpen() const noexcept
{
    return m_window && PlatformBackend::windowIsOpen(m_window);
}

void Window::close()
{
    if (m_window)
    {
        PlatformBackend::windowDestroy(m_window);
        m_window = nullptr;
    }
}

Window &Window::setTitle(const Cstring title) noexcept
{
    PlatformBackend::windowSetTitle(m_window, title);
    return *this;
}

Window& Window::setTitle(const String &title) noexcept
{
    PlatformBackend::windowSetTitle(m_window, title.c_str());
    return *this;
}

Cstring Window::getTitle() const noexcept
{
    return PlatformBackend::windowGetTitle(m_window);
}

Vec2i Window::getDisplaySize() const noexcept
{
    Vec2i size;
    PlatformBackend::windowGetDisplaySize(m_window, &size.x, &size.y);
    return size;
}

Window &Window::setSize(Vec2i size)
{
    PlatformBackend::windowSetSize(m_window, size.x, size.y);
    return *this;
}

Window &Window::setSize(int width, int height)
{
    PlatformBackend::windowSetSize(m_window, width, height);
    return *this;
}

Vec2i Window::getSize() const noexcept
{
    Vec2i size;
    PlatformBackend::windowGetSize(m_window, &size.x, &size.y);
    return size;
}

Float Window::getDPIScale() const noexcept
{
    int width, virtWidth;
    PlatformBackend::windowGetDisplaySize(m_window, &width, nullptr);
    PlatformBackend::windowGetSize(m_window, &virtWidth, nullptr);

    return static_cast<Float>(width) / static_cast<Float>(virtWidth);
}

Bool Window::isBorderless() const noexcept
{
    return PlatformBackend::windowIsBorderless(m_window);
}

Window &Window::setBorderless(const Bool value)
{
    PlatformBackend::windowSetBorderless(m_window, value);
    return *this;
}

Bool Window::isFullscreen() const noexcept
{
    return PlatformBackend::windowIsNativeFullscreen(m_window);
}

Window &Window::setFullscreen(const Bool value)
{
    if (isFullscreen() == value) return *this;

    PlatformBackend::windowSetNativeFullscreen(m_window, value);

    return *this;
}

Bool Window::isDesktopFullscreen() const noexcept
{
    return PlatformBackend::windowIsDesktopFullscreen(m_window);
}

Window &Window::setDesktopFullscreen(const Bool value)
{
    PlatformBackend::windowSetDesktopFullscreen(m_window, value);
    return *this;
}

Vec2i Window::getPosition() const noexcept
{
    Vec2i result;
    PlatformBackend::windowGetPosition(m_window, &result.x, &result.y);

    return result;
}

Recti Window::getDisplayRect() const noexcept
{
    Recti result;
    PlatformBackend::windowGetPosition(m_window, &result.x, &result.y);
    PlatformBackend::windowGetDisplaySize(m_window, &result.w, &result.h);

    return result;
}

Window &Window::maximize()
{
    PlatformBackend::windowMaximize(m_window);
    return *this;
}

Bool Window::isMaximized() const noexcept
{
    return PlatformBackend::windowIsMaximized(m_window);
}

Window &Window::minimize()
{
    PlatformBackend::windowMinimize(m_window);
    return *this;
}

Bool Window::isMinimized() const noexcept
{
    return PlatformBackend::windowIsMinimized(m_window);
}

Window &Window::restore()
{
    PlatformBackend::windowRestore(m_window);
    return *this;
}

Window &Window::setHidden(const Bool value)
{
    PlatformBackend::windowSetHidden(m_window, value);
    return *this;
}

Bool Window::isHidden() const noexcept
{
    return PlatformBackend::windowIsHidden(m_window);
}

Window& Window::setFloating(const Bool value)
{
    PlatformBackend::windowSetFloating(m_window, value);
    return *this;
}

Bool Window::isFloating() const noexcept
{
    return PlatformBackend::windowIsFloating(m_window);
}

Window &Window::setTransparent(Bool value)
{
    PlatformBackend::windowSetTransparent(m_window, value);
    return *this;
}

Bool Window::isTransparent() const noexcept
{
    return PlatformBackend::windowIsTransparent(m_window);
}

Window &Window::focus()
{
    PlatformBackend::windowFocus(m_window);
    return *this;
}

Bool Window::isFocused() const noexcept
{
    return PlatformBackend::windowIsFocused(m_window);
}


KAZE_NAMESPACE_END
