#pragma once

#include <kaze/core/lib.h>
#include <kaze/core/ImageContainer.h>
#include <kaze/core/WindowConstants.h>
#include <kaze/core/input/CursorConstants.h>
#include <kaze/core/input/MouseConstants.h>

KAZE_NS_BEGIN

namespace backend::cursor {
    /// Create a standard system cursor e.g. pointer, text input, etc.
    /// \param[in]  type        system cursor type to create
    /// \param[out] outCursor   the created cursor handle
    /// \returns whether the operation was successful.
    auto createStandard(CursorType type, CursorHandle *outCursor) noexcept -> bool;

    /// Create a custom cursor from image
    /// \param[in]  image      image to set the pixel data from
    /// \param[in]  anchorX    anchor x position in pixels within the image, where the click point is
    /// \param[in]  anchorY    anchor y position in pixels within the image, where the click point is
    /// \param[out] outCursor  retrieves the created cursor handle
    /// \returns whether the operation was successful.
    auto createCustom(const ImageContainer &image, int anchorX, int anchorY, CursorHandle *outCursor) noexcept -> bool;

    /// Set the cursor appearance
    /// \param[in]  window  window to set cursor on
    /// \param[in]  cursor  cursor to set
    /// \returns whether the operation succeeded.
    auto setCursor(WindowHandle window, CursorHandle cursor) noexcept -> bool;

    /// Get the current cursor
    /// \param[in]  window      window to get cursor from
    /// \param[out] outCursor   retrieves the current cursor handle
    /// \returns whether the retreival succeeded.
    auto getCursor(WindowHandle window, CursorHandle *outCursor) noexcept -> bool;

    /// Free cursor resources
    /// \param[in]  cursor   cursor to destroy
    /// \returns whether the operation succeeded.
    auto destroy(CursorHandle cursor) noexcept -> bool;

    /// Get the relative position of the mouse relative to a window.
    /// \param[in]  window  window to check
    /// \param[out] outX    retrieves the X position of the mouse relative to the window's position.
    /// \param[out] outY    retrieves the Y position of the mouse relative to the window's position.
    /// \returns whether the retrieval was successful.
    auto getRelativePosition(WindowHandle window, float *outX, float *outY) noexcept -> bool;

    /// Get the global screen position of the mouse cursor.
    /// \note if no windows are open, the result is undefined, false will be returned.
    /// \param[out] outX   retrieves global X position of mouse.
    /// \param[out] outY   retrieves global Y position of mouse.
    /// \returns whether the retrieval succeeded.
    auto getGlobalPosition(float *outX, float *outY) noexcept -> bool;

    /// Get whether a mouse button is down in a window
    /// \param[in]  window  window to check
    /// \param[in]  button  mouse button to check
    /// \param[out] outDown retrieves whether the button is down
    /// \returns whether the retrieval was succeessful.
    auto isDown(WindowHandle window, MouseBtn button, bool *outDown) noexcept -> bool;
}

KAZE_NS_END
