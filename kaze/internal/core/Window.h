/// \file Window.h
/// Contains Window class, which wraps a backend window for convenience and ease of use
/// via object-oriented encapsulation/RAII.
#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/WindowConstants.h>
#include <kaze/internal/core/input/CursorConstants.h>
#include <kaze/internal/core/math/Vec/Vec2.h>
#include <kaze/internal/core/math/Rect.h>

KAZE_NS_BEGIN

/// Represents a window and functionality about it
class Window {
public:
    // ----- lifetime ---------------------------------------------------------

    Window() noexcept;
    ~Window() noexcept;

    // no copying
    Window(const Window &other) = delete;
    auto operator=(const Window &other) -> Window & = delete;

    // move
    Window(Window &&other) noexcept;
    auto operator=(Window &&other) noexcept -> Window &;

    /// Emplace a window handle into a Window class, yet ownership is retained by the caller
    /// (the returned object will not clean up the handle via RAII). This is useful
    /// for enabling convenient access to window functionality when receiving a
    /// raw window pointer.
    /// \param[in] window window handle to pass
    /// \returns Window wrapper. User should not call `close()` with it,
    /// as it will result in an error. The original Window must make this call.
    static Window fromHandleRef(WindowHandle window) noexcept;

    /// Open a window, or re-open it with the new settings if it is already open
    /// \param[in] title window title text
    /// \param[in] width logical width of the window
    /// \param[in] height logical height of the window
    /// \param[in] initFlags window attributes to initialize with
    /// \returns whether the operation succeeded; a true return value means that
    ///          all other member functions in this class are ready to be called.
    auto open(
        Cstring title,
        Size    width,
        Size    height,
        WindowInit::Flags initFlags = WindowInit::None
    ) noexcept -> Bool;
    /// Checks if the window is currently open
    [[nodiscard]]
    auto isOpen() const noexcept -> Bool;
    /// Whether the window owns the internal window pointer
    [[nodiscard]]
    auto ownsWindow() const noexcept -> Bool;
    /// Closes a window if it's currently opened
    /// \returns whether window successfully closed, even on false, internal
    ///          window is reset, this value is just to explicitly report issues.
    auto close() noexcept -> bool;


    // ----- decoration -------------------------------------------------------

    /// Set the window's title bar text
    /// \param[in] title text to set
    /// \returns reference to this Window for chaining
    auto setTitle(Cstring title) noexcept -> Window &;

    /// Set the window's title bar text
    /// \param[in] title text to set
    /// \returns reference to this Window for chaining
    auto setTitle(const String &title) noexcept -> Window &;

    /// Get the window's title bar text
    [[nodiscard]]
    auto getTitle() const noexcept -> Cstring;

    /// Check whether the window is currently bordered (title bar, window controls)
    [[nodiscard]]
    auto isBordered() const noexcept -> Bool;

    /// Set whether the window is currently bordered
    /// \param[in] value `true` - window is bordered,
    ///              `false` - window is borderless
    /// \returns reference to this Window for chained calls.
    auto setBordered(Bool value) noexcept -> Window &;


    // ----- size & position --------------------------------------------------

    /// Set the size of the window in logical (virtual) units
    /// \param[in] size logical window size, packed into a Vec2i
    auto setSize(Vec2i size) noexcept -> Window &;

    /// Set the size of the window in logical (virtual) units
    /// \param[in] width logical window width to set
    /// \param[in] height logical window height to set
    /// \returns reference to this Window for chained calls.
    auto setSize(Int width, Int height) noexcept -> Window &;

    /// Get the size of the window in logical (virtual) units
    [[nodiscard]]
    auto getSize() const noexcept -> Vec2i;

    /// Get the current window position
    [[nodiscard]]
    auto getPosition() const noexcept -> Vec2i;

    /// Set the position of the window
    /// \param[in] position position to set
    /// \returns reference to this Window for chained calls.
    auto setPosition(Vec2i position) noexcept -> Window &;

    /// Get the display size of the window, in OS pixel units
    [[nodiscard]]
    auto getDisplaySize() const noexcept -> Vec2i;

    /// Get the display rectangle containing both position and size
    [[nodiscard]]
    auto getDisplayRect() const noexcept -> Recti;

    /// Get the ratio between the OS pixel size and the logical virtual size
    /// In systems that support high DPI, this usually is a factor > 1
    [[nodiscard]]
    auto getDPIScale() const noexcept -> Float;

    // ----- window behavior --------------------------------------------------

    /// Get whether the window is fullscreen or regular windowed
    [[nodiscard]]
    auto isFullscreen() const noexcept -> Bool;

    /// Set whether the window is fullscreen or regular windowed
    /// \param[in] value `true` - set window to fullscreen
    ///                  `false` - set window to windowed
    auto setFullscreen(Bool value) noexcept -> Window &;

    /// Get the fullscreen mode type
    [[nodiscard]]
    auto getFullscreenMode() const noexcept -> FullscreenMode;

    /// Set the fullscreen mode type
    /// \param[in] mode    fullscreen mode to set:
    ///     Desktop - remove bordered, make window take up entire screen
    ///     Native - utilize native platform for app fullscreen
    /// \returns reference to this Window for chained calls.
    auto setFullscreenMode(FullscreenMode mode) noexcept -> Window &;

    /// Maximize the window
    /// \returns reference to this Window for chained calls.
    auto maximize() noexcept -> Window &;

    /// Get whether the window is currently maximized
    [[nodiscard]]
    auto isMaximized() const noexcept -> Bool;

    /// Minimize the window
    /// \returns reference to this Window for chained calls.
    auto minimize() noexcept -> Window &;

    /// Check whether the window is currently minimized
    [[nodiscard]]
    auto isMinimized() const noexcept -> Bool;

    /// Restore a window from maximization or minimization
    /// \note on Mac, this doesn't appear to work from a minimized state,
    ///       probably due to the OS's method of window handling.
    /// \returns reference to this Window for chained calls.
    auto restore() noexcept -> Window &;

    /// Set whether the window should always float on top
    /// \param[in] value   `true` set window to float on top of others
    ///                    `false` set window to behave regularly–overlappable by others
    /// \returns wreference to this Window for chained calls.
    auto setFloating(Bool value) noexcept -> Window &;

    /// Get whether this window is set to always float on top of others.
    [[nodiscard]]
    auto isFloating() const noexcept -> Bool;

    /// Set whether this window is hidden or not
    /// \param[in] value   hidden setting:
    ///                    `true` hide window,
    ///                    `false` show window
    /// \returns reference to this Window for chained calls.
    auto setHidden(Bool value) noexcept -> Window &;

    /// Get whether the window is hidden or not
    [[nodiscard]]
    auto isHidden() const noexcept -> Bool;

    /// Set whether this window is set to have a transparent background (default: false)
    /// \param[in] value   transparency setting:
    ///                    `true` window background is capable of transparency (visible with transparent alpha clear color);
    ///                    `false` window has a solid background, non-transparent;
    /// \returns reference to this Window for chained calls.
    auto setTransparent(Bool value) noexcept -> Window &;

    /// Get whether the window is set to have a transparent background
    [[nodiscard]]
    auto isTransparent() const noexcept -> Bool;

    /// Force a window to be input-focused / raised into focus
    auto focus() noexcept -> Window &;

    /// Get whether a window is currently in focus
    [[nodiscard]]
    auto isFocused() const noexcept -> Bool;

    /// Set cursor mode for window.
    /// \param[in] mode  `CursorMode::Visible` - normal visibility
    ///                  `CursorMode::Hidden`  - hidden cursor
    ///                  `CursorMode::Capture` - cursor centered in screen, and values received
    ///                           from the MouseMotionEvent are relative;
    ///                  `false` - cursor is free to move, and values received from
    ///                       the cursor are positional (not relative)
    /// \returns reference to this Window for chained calls.
    auto setCursorMode(CursorMode mode) noexcept -> Window &;

    /// \returns cursor mode for window
    auto getCursorMode() const noexcept -> CursorMode;

    // ----- internal pointer -------------------------------------------------

    /// Get the internal window pointer
    [[nodiscard]]
    auto getHandle() const -> ConstWindowHandle;

    /// Get the internal window pointer
    [[nodiscard]]
    auto getHandle() -> WindowHandle;

private:
    WindowHandle m_window; ///< window handle
    bool m_ownsWindow;     ///< whether the Window owns its handle
};

KAZE_NS_END
