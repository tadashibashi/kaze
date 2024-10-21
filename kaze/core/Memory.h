/// \file memory.h
/// Low level memory functions
#pragma once
#ifndef kaze_core_memory_h_
#define kaze_core_memory_h_

#include <kaze/core/lib.h>

KAZE_NAMESPACE_BEGIN

namespace memory {

/// Allocate a number of bytes into a buffer
/// \param[in]  bytes   number of bytes to allocate
/// \returns buffer pointer or null on error.
[[nodiscard]]
auto alloc(Size bytes) noexcept -> void *;

/// Allocate space for a number of contiguous elements of type T
/// \note Memory is uninitialized and may contain junk data
/// \tparam    T         type of element
/// \param[in] elements  number of elements of type `T` to make space for;
///                      total size is `sizeof(T) * elements`
/// \returns buffer pointer or null on error.
template <typename T>
[[nodiscard]]
auto alloc(const Size elements) noexcept -> T *
{
    return static_cast<T *>(alloc(sizeof(T) * elements));
}

/// Reallocate data, may expand or shrink memory size
/// \param[in]  memory  pointer to the memory to resize
/// \param[in]  size    new size of the buffer
/// \returns reallocated pointer, or null on failure. If a failure, original `memory` is untouched.
[[nodiscard]]
auto realloc(void *memory, Size size) noexcept -> void *;

/// Free memory pointer allocated via `memory::alloc`
/// \param[in]  memory  block of memory to free
auto free(void *memory) noexcept -> void;

/// Allocate aligned memory
/// \param[in]  bytes      number of bytes to allocate (must be a multiple of `alignment`)
/// \param[in]  alignment  bytes of alignment (must be supported by the system platform)
/// \returns aligned memory pointer
[[nodiscard]]
auto allocAlign(Size bytes, Size alignment) noexcept -> void *;

/// Reallocate aligned memory
/// \param[in]  memory     aligned memory to reallocate
/// \param[in]  oldSize    size of `memory` (cannot be 0)
/// \param[in]  newSize    target resize size in bytes (cannot be 0, must be a multiple of alignment)
/// \param[in]  alignment  byte alignment
/// \returns resized byte-aligned memory buffer with copied data
[[nodiscard]]
auto reallocAlign(void *alignedMem, Size oldSize, Size newSize, Size alignment) noexcept -> void *;

/// If a function was called with allocAlign, it should be freed with freeAlign
/// \param[in]  alignedMemory   aligned memory to free
auto freeAlign(void *alignedMemory) noexcept -> void;

/// Copy a block of memory
/// \param[in] dest      pointer to copy data into
/// \param[in] source    pointer to source data to copy
/// \param[in] byteCount number of bytes to copy from `source` into `dest`
auto copy(void *dest, const void *source, Size byteCount) noexcept -> void;

/// Set a block of memory
/// \param[in]  memory  pointer to the data to set
/// \param[in]  value   value to set the memory to
/// \param[in]  size    size of the memory to set
auto set(void *memory, Int value, Size size) noexcept -> void;



}


KAZE_NAMESPACE_END

#endif // kaze_core_memory_h_
