#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/video/WindowConstants.h>

KAZE_NAMESPACE_BEGIN

namespace backend::window {
    /// Window native platform pointers
    struct NativePlatformData {
        void *windowHandle;
        void *displayType;
    };

    /// Open/create a new window
    /// \param[in]  title      title to display on the window's title bar
    /// \param[in]  width      window's logical width
    /// \param[in]  height     window's logical height
    /// \param[in]  flags      specific attributes to initialize the window with
    /// \param[out] outWindow  receives the created window handle; required!
    /// \returns window handle pointer; on error, a `nullptr` is returned.
    auto open(
        const char *title,
        int width, int height,
        WindowInit::Flags flags,
        WindowHandle *outWindow) noexcept -> bool;

    /// Close/destroy a window opened with `window::open`.
    /// All internals are freed, so the pointer is thereafter safe to discard.
    /// \param[in] window   window to close
    /// \returns whether the window was sucessfully closed; false will be returned on an invalid or invalidated
    ///          handle. However, any window that is valid is guaranteed to close and the window pointer safe to
    ///          discard after calling this function.
    auto close(WindowHandle window) noexcept -> bool;

    /// Get native platform-specific information about the window for use with bgfx
    /// Each platform the backend intends to support should be taken into account with pre-processor checks
    /// either from the backend library or via kaze's platform defines.
    /// \param[in] window   window to get info from
    /// \returns platform pointers for bgfx
    auto getNativeInfo(WindowHandle window) noexcept -> NativePlatformData;

    /// Check if a window is open.
    /// \param [in]  window     window to check
    /// \param [out] outIsOpen  retrieves whether window is open
    /// \returns whether the retrieval succeeded
    auto isOpen(WindowHandle window, bool *outIsOpen) noexcept -> bool;

    /// Set user data to associate with the WindowHandle
    /// \param[in] window     window to set userdata on
    /// \param[in] userdata   pointer to set
    /// \returns whether the operation succeeded
    auto setUserData(WindowHandle window, void *userdata) noexcept -> bool;

    /// Get the user pointer associated with the WindowHandle.
    /// \param[in]  window      window to check
    /// \param[out] outUserdata retrieves user data pointer currently associated with the window
    /// \returns whether the retrieval succeeded; on error `outUserdata` will not be modified
    auto getUserData(WindowHandle window, void **outUserdata) noexcept -> bool;

    /// Set the title to be displayed on the window's title bar.
    /// \param[in] window window to affect
    /// \param[in] title  null-terminated title message to set
    /// \returns whether the operation succeeded
    auto setTitle(WindowHandle window, const char *title) noexcept -> bool;

    /// Get the title currently displayed on the window bar.
    /// \param[in]  window   window to check
    /// \param[out] outTitle retrieves the window's current title string
    /// \returns whether the operation succeeded
    auto getTitle(WindowHandle window, const char **outTitle) noexcept -> bool;

    /// Set the logical size of the window
    /// \param[in] window   window to affect
    /// \param[in] width    logical width of the window to set
    /// \param[in] height   logical height of the window to set
    /// \returns whether the operation succeeded
    auto setSize(WindowHandle window, int width, int height) noexcept -> bool;

    /// Get the logical size of the window
    /// \param[in]  window     window to check
    /// \param[out] outWidth   retrieves the logical width of the window; may be `nullptr`
    /// \param[out] outHeight  retrieves the logical height of the window; may be `nullptr`
    /// \returns whether the retrieval succeeded; on error, `width` and `height` are not modified.
    auto getSize(WindowHandle window, int *outWidth, int *outHeight) noexcept -> bool;

    /// Get the true OS pixel size of the window on the monitor.
    /// On high-DPI monitors, this may differ from the logical size retrieved via `getSize`.
    /// \param[in]  window     window to check
    /// \param[out] outWidth   retrieves the framebuffer width in actual OS pixel units; may be `nullptr`
    /// \param[out] outHeight  retrieves the framebuffer height in actual OS pixel units; may be 'nullptr'
    /// \returns whether the operation succeeded; on error, `x` and `y` are not modified.
    auto getFramebufferSize(WindowHandle window, int *outWidth, int *outHeight) noexcept -> bool;

    /// Get whether window is fullscreen
    /// \param[in]  window        window to check
    /// \param[out] outFullscreen retrives whether the window is currently in fullscreen mode
    /// \returns whether the retrieval was successful; on error, `outFullscreen` is not modified.
    auto isFullscreen(WindowHandle window, bool *outFullscreen) noexcept -> bool;

    /// Set fullscreen mode for window. Default is `FullscreenMode::Native`.
    /// \param[in] window  window to affect
    /// \param[in] mode    fullscreen mode to set
    /// \returns whether the operation was successful
    auto setFullscreenMode(WindowHandle window, FullscreenMode mode) noexcept -> bool;

    /// Get fullscreen mode for window. Default is `FullscreenMode::Native`.
    /// \param[in]  window  window to check
    /// \param[out] outMode retrieves the mode
    /// \returns whether the retrieval was successful
    auto getFullscreenMode(WindowHandle window, FullscreenMode *outMode) noexcept -> bool;

    /// Activate window fullscreen mode or restore back to normal.
    /// \param[in] window   window to affect
    /// \param[in] value    true sets window into fullscreen mode, false restores it to regular windowed mode.
    /// \returns whether operation was successful
    auto setFullscreen(WindowHandle window, bool value) noexcept -> bool;

    /// Get whether window is bordered (title bar, border, and window controls)
    /// \param[in]  window   window to check
    /// \param[out] outBordered whether window is bordered
    /// \returns whether retrieval was successful
    auto isBordered(WindowHandle window, bool *outBordered) noexcept -> bool;

    /// Set window bordered (title bar, border, and window controls) or remove it
    /// \param[in] window window to affect
    /// \param[in] bordered  true sets window bordered, false removes it
    /// \returns whether the operation was successful
    auto setBordered(WindowHandle window, bool bordered) noexcept -> bool;

    /// Get whether window is currently minimized
    /// \param[in]  window     window to check
    /// \param[out] minimized  retrieves whether the window is currently minimized
    /// \returns whether the retrieval was successful
    auto isMinimized(WindowHandle window, bool *minimized) noexcept -> bool;

    /// Minimize a window
    /// \param[in] window window to affect
    /// \returns whether operation was successful
    auto minimize(WindowHandle window) noexcept -> bool;

    /// Check whether window is currently maximized
    /// \param[in]  window     window to check
    /// \param[out] maximized  retrieves whether window is currently maximized
    /// \returns whether the retrieval was successful
    auto isMaximized(WindowHandle window, bool *maximized) noexcept -> bool;

    /// Maximize a window
    /// \param[in] window   window to affect
    /// \returns whether operation was successful
    auto maximize(WindowHandle window) noexcept -> bool;

    /// Restore a window from being minimized or maximized, back to normal
    /// \param[in] window   window to affect
    /// \returns whether operation was successful
    auto restore(WindowHandle window) noexcept -> bool;

    /// Get a window's screen position, the position of its top-left corner.
    /// \param[in]  window   window to check
    /// \param[out] x        window's x screen position
    /// \param[out] y        window's y screen position
    /// \returns whether operation was successful; on error, `x` and `y` will not be modified.
    auto getPosition(WindowHandle window, int *x, int *y) noexcept -> bool;

    /// Set a window's screen position, the position of its top-left corner.
    /// \param[in]  window   window to affect
    /// \param[in]  x        x screen position to set
    /// \param[in]  y        y screen position to set
    /// \returns whether operation was successful
    auto setPosition(WindowHandle window, int x, int y) noexcept -> bool;

    /// Get whether a window is resizable.
    /// \param[in]  window        window to check
    /// \param[out] outResizable  retrieves whether window is resizable
    /// \returns whether retrieval succeeded; on error, `outResizable` will not be modified.
    auto getResizable(WindowHandle window, bool *outResizable) noexcept -> bool;

    /// Set window resizability.
    /// \param[in] window      window to affect
    /// \param[in] resizable   `true` makes the window user-resizable;
    ///                        `false` prevents user resizing.
    /// \returns whether operation succeeded
    auto setResizable(WindowHandle window, bool resizable) noexcept -> bool;

    /// Get whether a window is hidden.
    /// \param[in]  window    window to check
    /// \param[out] outHidden retrieves whether window is hidden
    /// \returns whether retrieval succeeded
    auto isHidden(WindowHandle window, bool *outHidden) noexcept -> bool;

    /// Set whether a window is hidden (or visible)
    /// \param[in] window   window to affect
    /// \param[in] hidden   `true` makes window hidden,
    ///                     `false` makes window visible.
    /// \returns whether operation succeeded.
    auto setHidden(WindowHandle window, bool hidden) noexcept -> bool;

    /// Get whether a window is currently hovered by a cursor
    /// \param[in]  window       window to check
    /// \param[out] outHovered   retrieves whether window is currently hovered by a cursor or not
    /// \returns whether retrieval succeeded
    auto isHovered(WindowHandle window, bool *outHovered) noexcept -> bool;

    /// Get whether a window is currently in floating mode (always on top)
    /// \param[in]  window      window to check
    /// \param[out] outFloating retrieves whether window is in floating mode
    /// \returns whether retrieval succeeded
    auto isFloating(WindowHandle window, bool *outFloating) noexcept -> bool;

    /// Set a window's floating mode (always on top)
    /// \param[in] window    window to affect
    /// \param[in] floating  `true` makes window always float on top of others,
    ///                      `false` sets window to behave normally.
    /// \returns whether operation succeeded.
    auto setFloating(WindowHandle window, bool floating) noexcept -> bool;

    /// Get whether a window has transparency enabled
    /// \param[in]   window          window to check
    /// \param[out]  outTransparent  retrieves whether the window has transparency enabled
    /// \returns whether the retrieval succeeded.
    auto isTransparent(WindowHandle window, bool *outTransparent) noexcept -> bool;

    /// Set a window's transparency mode
    /// \param[in] window    window to affect
    /// \param[in] transparent `true` enables window transparency,
    ///                    `false` makes window fully opaque.
    /// \returns whether the operation succeeded.
    auto setTransparent(WindowHandle window, bool transparent) noexcept -> bool;

    /// Get whether a window is currently focused by an input source (keyboard, mouse)
    /// \param[in]  window      window to check
    /// \param[out] outFocused  retrieves whether window is currently input focused
    /// \returns whether the retrieval succeeded.
    auto isFocused(WindowHandle window, bool *outFocused) noexcept -> bool;

    /// Force a window into focus
    /// \param[in] window window to affect
    /// \returns whether operation succeeded.
    auto focus(WindowHandle window) noexcept -> bool;

    /// Set minimum logical size of window
    /// \param[in] window    window to affect
    /// \param[in] minWidth  minimum width of window to set (in logical units)
    /// \param[in] minHeight minimum height of window to set (in logical units)
    /// \returns whether the operation succeeded.
    auto setMinSize(WindowHandle window, int minWidth, int minHeight) noexcept -> bool;

    /// Set maximum logical size of window
    /// \param[in] window    window to affect
    /// \param[in] maxWidth  maximum width of window to set (in logical units)
    /// \param[in] maxHeight maximum height of window to set (in logical units)
    /// \returns whether the operation succeeded.
    auto setMaxSize(WindowHandle window, int maxWidth, int maxHeight) noexcept -> bool;

    /// Get minimum logical size of window
    /// \param[in]  window       window to check
    /// \param[out] outMinWidth  retrieves current minimum width of window, or -1 if there is no such bounds set
    /// \param[out] outMinHeight retrieves current minimum height of window, or -1 if there is no such bounds set
    /// \returns whether the retrieval succeeded.
    auto getMinSize(WindowHandle window, int *outMinWidth, int *outMinHeight) noexcept -> bool;

    /// Get maximum logical size of window
    /// \param[in]  window       window to check
    /// \param[out] outMaxWidth  retrieves current maximum width of window, or -1 if there is no such bounds set
    /// \param[out] outMaxHeight retrieves current maximum height of window, or -1 if there is no such bounds set
    /// \returns whether the retrieval succeeded.
    auto getMaxSize(WindowHandle window, int *outMaxWidth, int *outMaxHeight) noexcept -> bool;

    /// Set whether to show cursor in window
    /// \param[in] window   window to affect
    /// \param[in] show     `true` shows the cursor when it hovers over the window,
    ///                 `false` hides the cursor when it hovers over the window.
    /// \returns whether the operation succeeded.
    auto setShowCursorMode(WindowHandle window, bool show) noexcept -> bool;

    /// Get cursor visibility mode–whether the mouse cursor is shown when hovering over the window
    /// \param[in]  window   window to check
    /// \param[out] outShow  retrieves the mode
    /// \returns whether the retrieval succeeded.
    auto getShowCursorMode(WindowHandle window, bool *outShow) noexcept -> bool;

    /// Set the cursor capture mode of the window.
    /// Capture mode hides the cursor, keeps it centered on screen, and processes mouse motion events relatively.
    /// When set to false, mouse motion events are processed positionally, relative to the window position.
    /// \param[in] window   window to affect
    /// \param[in] capture  whether to set capture mode for the window
    /// \returns whether the operation succeeded.
    auto setCaptureCursorMode(WindowHandle window, bool capture) noexcept -> bool;

    /// Get the cursor capture mode of the window.
    /// See {@link setCaptureCursorMode} for more info on capture mode.
    /// \param[in]  window      window to check
    /// \param[out] outCapture  retrieves current capture mode.
    /// \returns whether the retrieval succeeded.
    auto getCaptureCursorMode(WindowHandle window, bool *outCapture) noexcept -> bool;

    /// Set whether text input events are enabled for a window.
    /// \note When active, IME keyboard should be opened if supported on the platform,
    ///       and when not it should disengage (Unsupported on GLFW3!)
    /// \param[in]  window   window to set text input mode on
    /// \param[in]  active   `true` makes text input events active, `false` turns them off.
    /// \returns whether operation succeeded
    auto setTextInputMode(WindowHandle window, bool active) noexcept -> bool;

    /// Check if text input events are enabled for a window
    /// \param[in]   window    window to check
    /// \param[out]  outValue  retrieves whether text input events are enabled
    /// \returns whether the retrieval succeeded
    auto isTextInputActive(WindowHandle window, bool *outValue) noexcept -> bool;
}

KAZE_NAMESPACE_END
