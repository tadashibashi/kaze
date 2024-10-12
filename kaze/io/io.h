#pragma once
#ifndef kaze_io_io_h_
#define kaze_io_io_h_

#include <kaze/kaze.h>
#include <kaze/memory.h>

KAZE_NAMESPACE_BEGIN

/// Load a file from the file system
/// @param[in]  path     path to the file
/// @param[out] outData  retrieves the pointer the loaded file data;
///                      make sure to free this later via `kaze::release`.
/// @param[out] outSize  retrieves the size of the data received
/// @returns whether operation succeeded.
auto loadFile(Cstring path, Ubyte **outData, Size *outSize) -> Bool;

/// Write a buffer to a system file. The previous contents are truncated (erased)
/// @param[in]  path    path to the file to write (does not need to exist yet)
/// @param[in]  data    pointer to the source data buffer
/// @param[in]  size    size of the data buffer in bytes
auto writeFile(Cstring path, void *data, Size size) -> Bool;

KAZE_NAMESPACE_END

#endif // kaze_io_io_h_
