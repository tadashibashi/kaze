#pragma once
#ifndef kaze_core_input_cursor_h_
#define kaze_core_input_cursor_h_

#include <kaze/core/lib.h>
#include <kaze/core/input/CursorConstants.h>

KAZE_NAMESPACE_BEGIN

class Cursor {

};

class CursorMgr {
public:
    auto create(CursorType type) -> Cursor *;

};

KAZE_NAMESPACE_END


#endif // kaze_core_input_cursor_h_
