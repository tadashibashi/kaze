#include "memory.h"
#include <kaze/core/debug.h>
#include <kaze/core/platform/defines.h>

#include <cstdlib>
#include <cstring>

KAZE_NS_BEGIN

namespace memory {

auto alloc(const Size bytes) noexcept -> void *
{
    if (bytes == 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "attempted to alloc 0 bytes");
        return nullptr;
    }

    const auto buffer = std::malloc(bytes);
    if ( !buffer )
    {
        KAZE_PUSH_ERR(Error::OutOfMemory, "Out of memory");
        return nullptr;
    }

    return buffer;
}

auto allocAlign(Size bytes, Size alignment) noexcept -> void *
{
    if (bytes == 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "Attempted to allocate 0 bytes");
        return nullptr;
    }

    KAZE_ASSERT(alignment > 0);
    KAZE_ASSERT(bytes % alignment == 0, "`bytes` must be a multiple of `alignment`");

#if KAZE_PLATFORM_WINDOWS
    return _aligned_malloc(bytes, alignment);
#else
    return std::aligned_alloc(alignment, bytes);
#endif
}

auto free(void *memory) noexcept -> void
{
    std::free(memory);
}

auto freeAlign(void *alignedMemory) noexcept -> void
{
#if KAZE_PLATFORM_WINDOWS
    _aligned_free(alignedMemory);
#else
    std::free(alignedMemory);
#endif
}

auto copy(void *target, const void *source, Size byteCount) noexcept -> void
{
    if (byteCount == 0) return;

    std::memcpy(target, source, byteCount);
}

auto set(void *memory, Int value, Size size) noexcept -> void
{
    if (size == 0) return;

    std::memset(memory, value, size);
}

auto realloc(void *memory, Size size) noexcept -> void *
{
    if ( !memory )
    {
        KAZE_PUSH_ERR(Error::NullArgErr, "required arg `memory` was null");
        return nullptr;
    }

    if (size == 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "realloc size must be > 0");
        return nullptr;
    }

    return std::realloc(memory, size);
}

auto reallocAlign(void *alignedMem, Size oldSize, Size newSize, Size alignment) noexcept -> void *
{
    KAZE_ASSERT((uintptr_t)alignedMem % alignment == 0,
        fmt_lib::format("memory passed to `reallocAlign` must have an `alignment` of: {}", alignment));

    if ( !alignedMem )
    {
        KAZE_PUSH_ERR(Error::NullArgErr, "cannot reallocate null `memory`");
        return nullptr;
    }

    if (oldSize == 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "cannot reallocate 0 bytes");
        return nullptr;
    }

    if (newSize == 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "cannot shrink buffer to 0 bytes");
        return nullptr;
    }

    if (oldSize == newSize)
        return alignedMem;

    auto newMem = memory::allocAlign(newSize, alignment);
    if ( !newMem )
        return nullptr;

    memory::copy(newMem, alignedMem, newSize > oldSize ? oldSize : newSize);
    memory::freeAlign(alignedMem);
    return newMem;
}


}


KAZE_NS_END

