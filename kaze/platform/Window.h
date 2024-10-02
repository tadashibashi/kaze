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
        Resizable = 1u << 0,   ///< user can drag corners to adjust the window size
        Borderless = 1u << 1,  ///< has no border, title bar, or window control buttons
        Fullscreen = 1u << 2,  ///< starts off native fullscreen
        Floating = 1u << 3,    ///< exhibits always on-top behavior
        Transparent = 1u << 4, ///< framebuffer is see-through
        Hidden = 1u << 5,      ///< starts off hidden
        Maximized = 1u << 6,   ///< starts off maximized
    };
};

enum struct FullscreenMode
{
    Native,  ///< uses the platform's native windowing API for fullscreen
    Desktop, ///< on fullscreen, window becomes borderless, positioned at the top-left, with full screen size
    Unknown,
};

constexpr WindowInit::Flags operator |(const WindowInit::Flags a, const WindowInit::Flags b)
{
    return static_cast<WindowInit::Flags>(static_cast<Uint>(a) | static_cast<Uint>(b));
}

class Window {
public:
    Window() noexcept;
    ~Window() noexcept;

    /// Open a window, or re-open it with the new settings if it is already open
    Bool open(Cstring title, Size width, Size height, WindowInit::Flags initFlags = WindowInit::None) noexcept;
    /// Checks if the window is currently open
    [[nodiscard]] Bool isOpen() const noexcept;
    /// Closes a window if it's currently opened
    /// @returns whether window successfully closed, even on false, internal window is reset, this value is just to
    ///          explicitly report issues.
    Bool close() noexcept;

    /// Set the window's title bar text
    Window &setTitle(Cstring title) noexcept;
    Window &setTitle(const String &title) noexcept;

    /// Get the window's title bar text
    [[nodiscard]] Cstring getTitle() const noexcept;

    /// Get the display size of the window, in OS pixel units
    [[nodiscard]] Vec2i getDisplaySize() const noexcept;

    /// Set the size of the window in logical (virtual) units
    Window &setSize(Vec2i size) noexcept;
    /// Set the size of the window in logical (virtual) units
    Window &setSize(int width, int height) noexcept;
    /// Get the size of the window in logical (virtual) units
    [[nodiscard]] Vec2i getSize() const noexcept;

    /// Get the ratio between the OS pixel size and the logical virtual size
    /// In systems that support high DPI, this usually is a factor > 1
    [[nodiscard]] Float getDPIScale() const noexcept;

    [[nodiscard]] Bool isBordered() const noexcept;
    Window &setBordered(Bool value) noexcept;

    [[nodiscard]] Bool isFullscreen() const noexcept;
    Window &setFullscreen(Bool value) noexcept;

    [[nodiscard]] FullscreenMode getFullscreenMode() const noexcept;
    Window &setFullscreenMode(FullscreenMode mode) noexcept;

    [[nodiscard]] Vec2i getPosition() const noexcept;
    [[nodiscard]] Recti getDisplayRect() const noexcept;

    Window &maximize() noexcept;
    [[nodiscard]] Bool isMaximized() const noexcept;
    Window &minimize() noexcept;
    [[nodiscard]] Bool isMinimized() const noexcept;
    Window &restore() noexcept;

    Window &setHidden(Bool value) noexcept;
    [[nodiscard]] Bool isHidden() const noexcept;

    Window &setFloating(Bool value) noexcept;
    [[nodiscard]] Bool isFloating() const noexcept;

    /// @note Not all platforms support this feature
    Window &setTransparent(Bool value) noexcept;
    [[nodiscard]] Bool isTransparent() const noexcept;

    Window &focus() noexcept;
    [[nodiscard]] Bool isFocused() const noexcept;

private:
    void *m_window;
};



KAZE_NAMESPACE_END

#endif // kaze_platform_window_h_
