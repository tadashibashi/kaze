#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/AlignedAllocator.h>

KAZE_NAMESPACE_BEGIN

template <typename T, Size Alignment>
using AlignedList = List<T, AlignedAllocator<T, Alignment>>;

KAZE_NAMESPACE_END
