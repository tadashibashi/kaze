#pragma once
#ifndef kaze_core_input_mouseconstants_h_
#define kaze_core_input_mouseconstants_h_

#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN

enum class MouseAxis
{
    ScrollX,
    ScrollY,
    Count
};

enum class MouseBtn
{
    Left,
    Right,
    Middle,
    Ext1,
    Ext2,
    Ext3,
    Ext4,
    Ext5,
    Count
};

KAZE_NAMESPACE_END

#endif // kaze_core_input_mouseconstants_h_
