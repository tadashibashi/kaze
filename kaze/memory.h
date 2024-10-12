#pragma once
#ifndef kaze_memory_h_
#define kaze_memory_h_

#include <kaze/kaze.h>

KAZE_NAMESPACE_BEGIN

/// Allocate a number of bytes into a buffer
/// @returns buffer pointer or null on error.
auto alloc(Size bytes) -> void *;

/// Allocate space for a number of elements of type T
/// @note Memory is uninitialized.
/// @returns buffer pointer or null on error.
template <typename T>
auto alloc(const Size elements) -> T *
{
    return static_cast<T *>(alloc(sizeof(T) * elements));
}

/// Free memory pointer allocated via `alloc`
auto release(void *memory) -> void;

/// Copy a block of memory
/// @param[in] dest      pointer to copy data into
/// @param[in] source    pointer to source data to copy
/// @param[in] byteCount number of bytes to copy from `source` into `dest`
auto copy(void *dest, const void *source, Size byteCount) -> void;

KAZE_NAMESPACE_END

#endif // kaze_memory_h_
