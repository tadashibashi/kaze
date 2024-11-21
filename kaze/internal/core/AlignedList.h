#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/AlignedAllocator.h>

KAZE_NS_BEGIN

template <typename T, Size Alignment>
using AlignedList = List<T, AlignedAllocator<T, Alignment>>;

KAZE_NS_END
