#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/AlignedAllocator.h>

KAZE_NS_BEGIN

template <typename T, Size Alignment>
using AlignedList = List<T, AlignedAllocator<T, Alignment>>;

KAZE_NS_END
