#pragma once

#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/memory.h>
#include <kaze/internal/core/MemView.h>

KAZE_NS_BEGIN

namespace file {
    /// Load a file from the file system
    /// \param[in]  path     path to the file
    /// \param[out] outData  retrieves the pointer of the loaded file data;
    ///                      make sure to free this later via `kaze::release`.
    /// \param[out] outSize  retrieves the size of the data received
    /// \returns whether operation succeeded.
    auto load(StringView path, Ubyte **outData, Size *outSize) -> Bool;

    /// Write a buffer to a system file. The previous contents are truncated (erased)
    /// \param[in]  path    path to the file to write (does not need to exist yet)
    /// \param[in]  mem     memory to write
    /// \returns whether operation succeeded
    auto write(StringView path, Mem mem) -> Bool;
}

KAZE_NS_END
