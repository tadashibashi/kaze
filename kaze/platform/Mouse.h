#pragma once
#ifndef kaze_platform_mouse_h_
#define kaze_platform_mouse_h_

#include <kaze/kaze.h>

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

#endif // kaze_platform_mouse_h_