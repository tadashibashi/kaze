#include "memory.h"
#include <cstdlib>
#include <kaze/debug.h>

KAZE_NAMESPACE_BEGIN

auto alloc(const Size bytes) -> void *
{
    const auto buffer = std::malloc(bytes);
    if ( !buffer )
    {
        KAZE_CORE_ERRCODE(Error::OutOfMemory, "Out of memory");
        return nullptr;
    }

    return buffer;
}

auto free(void *memory) -> void
{
    std::free(memory);
}

auto copy(void *target, const void *source, Size byteCount) -> void
{
    std::memcpy(target, source, byteCount);
}

KAZE_NAMESPACE_END

