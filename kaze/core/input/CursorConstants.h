#pragma once
#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN

/// Intermediary Cursor type
struct CursorHandle {
    void *handle;
};

/// Cursor visibility and behavior mode
enum class CursorMode {
    /// Cursor is visible in window
    Visible,

    /// Cursor is invisible in window
    Hidden,

    /// Cursor is captured inside window and made invisible, with a focus on relative positions in `MouseMotionEvent`.
    /// This is useful for first-person types of games. Mouse position is undefined, but the
    /// relative position should be used in the event.
    Relative,

    /// Number of cursor modes in this enum, also a default "unknown" value
    Count,
};

/// Standard OS cursor types
enum class CursorType {
    Arrow,      ///< standard default arrow
    TextInput,  ///< for hovers over text entry areas, also known as "I-beam"
    Crosshair,  ///< plus-sign
    Move,       ///< arrows in NESW directions, also known as "resize all"
    ResizeV,    ///< arrows in NS vertical direction
    ResizeH,    ///< arrows in EW horizontal direction
    ResizeTLBR, ///< top-left to bottom-right arrows
    ResizeTRBL, ///< top-right to bottom-left arrows
    Pointer,    ///< pointer hand
    NotAllowed, ///< no sign
    Count       ///< number of cursor types, also a default "unknown" value
};

KAZE_NAMESPACE_END
