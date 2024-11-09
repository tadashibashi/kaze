#pragma once

#include <kaze/core/lib.h>

KAZE_NS_BEGIN

enum class MouseAxis
{
    Unknown = -1,
    ScrollX,
    ScrollY,
    Count
};

enum class MouseBtn
{
    Unknown = -1,
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

KAZE_NS_END
