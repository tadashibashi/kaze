#pragma once

#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN

struct CursorHandle {
    void *handle;
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
