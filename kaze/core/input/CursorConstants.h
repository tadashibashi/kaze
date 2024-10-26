#pragma once

#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN

struct CursorHandle {
    void *handle;
};

/// Cursor visibility and behavior mode
enum class CursorMode {
    /// Cursor is visible in window
    Visible,

    /// Cursor is invisible in window
    Hidden,

    /// Cursor is captured inside window, invisible and passes relative positions to `MouseMotionEvent`
    Capture,

    /// Number of cursor modes in this enum, also a default "unknown" value
    Count,
};

enum class CursorType {
    Default,
    TextInput,
    Crosshair,
    Move,
    ResizeV,
    ResizeH,
    ResizeTLBR,
    ResizeTRBL,
    Pointer,
    NotAllowed,
    Count
};

KAZE_NAMESPACE_END
