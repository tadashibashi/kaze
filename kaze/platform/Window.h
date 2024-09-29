#pragma once

#ifndef kaze_platform_window_h_
#define kaze_platform_window_h_

#include <kaze/kaze.h>
#include <kaze/math/Vec/Vec2.h>
#include <kaze/math/Rect.h>

KAZE_NAMESPACE_BEGIN

struct WindowInit {
    enum Flags : Uint
    {
        None = 0,
        Resizable = 1u << 0,
        Borderless = 1u << 1,
        Fullscreen = 1u << 2,
        Floating = 1u << 3,
        Transparent = 1u << 4,
        Hidden = 1u << 5,
        Maximized = 1u << 6,
    };
};

constexpr WindowInit::Flags operator |(const WindowInit::Flags a, const WindowInit::Flags b)
{
    return static_cast<WindowInit::Flags>(static_cast<Uint>(a) | static_cast<Uint>(b));
}

class Window {
public:
    Window();
    ~Window();

    /// Open a window, or re-open it with the new settings if it is already open
    Bool open(Cstring title, Size width, Size height, WindowInit::Flags initFlags = WindowInit::None);
    /// Checks if the window is currently open
    [[nodiscard]] Bool isOpen() const noexcept;
    /// Closes a window if it's currently opened (a no-op otherwise)
    void close();

    /// Set the window's title bar text
    Window &setTitle(Cstring title) noexcept;
    Window &setTitle(const String &title) noexcept;

    /// Get the window's title bar text
    [[nodiscard]] Cstring getTitle() const noexcept;

    /// Get the display size of the window, in OS pixel units
    [[nodiscard]] Vec2i getDisplaySize() const noexcept;

    /// Set the size of the window in logical (virtual) units
    Window &setSize(Vec2i size);
    /// Set the size of the window in logical (virtual) units
    Window &setSize(int width, int height);
    /// Get the size of the window in logical (virtual) units
    [[nodiscard]] Vec2i getSize() const noexcept;

    /// Get the ratio between the OS pixel size and the logical virtual size
    /// In systems that support high DPI, this usually is a factor > 1
    [[nodiscard]] Float getDPIScale() const noexcept;

    [[nodiscard]] Bool isBorderless() const noexcept;
    Window &setBorderless(Bool value);

    [[nodiscard]] Bool isFullscreen() const noexcept;
    Window &setFullscreen(Bool value);

    [[nodiscard]] Bool isDesktopFullscreen() const noexcept;
    Window &setDesktopFullscreen(Bool value);

    [[nodiscard]] Vec2i getPosition() const noexcept;
    [[nodiscard]] Recti getDisplayRect() const noexcept;

    Window &maximize();
    [[nodiscard]] Bool isMaximized() const noexcept;
    Window &minimize();
    [[nodiscard]] Bool isMinimized() const noexcept;
    Window &restore();

    Window &setHidden(Bool value);
    [[nodiscard]] Bool isHidden() const noexcept;

    Window &setFloating(Bool value);
    [[nodiscard]] Bool isFloating() const noexcept;

    /// @note Not all platforms support this feature
    Window &setTransparent(Bool value);
    [[nodiscard]] Bool isTransparent() const noexcept;

    Window &focus();
    [[nodiscard]] Bool isFocused() const noexcept;

private:
    void *m_window;
};



KAZE_NAMESPACE_END

#endif // kaze_platform_window_h_
