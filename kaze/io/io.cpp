#include "io.h"
#include <kaze/debug.h>
#include <kaze/memory.h>

#include <filesystem>
#include <fstream>

KAZE_NAMESPACE_BEGIN
const int BytesPerRead = 1024;

auto loadFile(Cstring path, Ubyte **outData, Size *outSize) -> Bool
{
    if ( !path )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `path` was null");
        return KAZE_FALSE;
    }

    if ( !outData )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `outData` was null");
        return KAZE_FALSE;
    }

    if ( !outSize )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `outSize` was null");
    }

    std::ifstream file(path, std::ios::binary | std::ios::in);

    if ( !file.is_open() )
    {
        KAZE_CORE_ERRCODE(Error::FileOpenErr, "Failed to open file at '{}'", path);
        return KAZE_FALSE;
    }

    // get the file size
    Size byteLength = 0;
    if ( !file.seekg(0, std::ios::end) )
    {
        KAZE_CORE_ERRCODE(Error::FileSeekErr, "Failed to seek to end of file");
        return KAZE_FALSE;
    }

    Ubyte *data = alloc<Ubyte>(byteLength);
    if ( !data )
    {
        return KAZE_FALSE;
    }

    byteLength = file.tellg();
    Size b = 0;
    for (const Int64 limit = static_cast<Int64>(byteLength) - static_cast<Int64>(BytesPerRead); b <= limit; b += BytesPerRead)
    {
        if ( !file.read((char *)&data[b], BytesPerRead) )
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failure during file read call");
            return KAZE_FALSE;
        }
    }

    // Catch leftovers
    if (b < byteLength)
    {
        if ( !file.read((char *)&data[b], byteLength - b) )
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failure during file read call");
            return KAZE_FALSE;
        }
    }

    *outData = data;
    *outSize = byteLength;
    return KAZE_TRUE;
}

auto writeFile(Cstring path, void *data, Size size) -> Bool
{
    if (const auto fsPath = std::filesystem::path(path); fsPath.has_parent_path())
    {
        if (const auto parent = fsPath.parent_path(); !std::filesystem::exists(fsPath.parent_path()))
        {
            std::filesystem::create_directories(fsPath.parent_path());
        }
    }

    std::ofstream file(path, std::ios::out | std::ios::trunc | std::ios::binary);
    if ( !file.is_open() )
    {
        KAZE_CORE_ERRCODE(Error::FileOpenErr, "Failed to open file for writing at: {}", path);
        return KAZE_FALSE;
    }

    Size b = 0;
    for (const Int64 limit = static_cast<Int64>(size) - static_cast<Int64>(BytesPerRead); b <= limit; b += BytesPerRead)
    {
        if ( !file.write((char *)(data) + b, BytesPerRead) )
        {
            KAZE_CORE_ERRCODE(Error::FileWriteErr, "Failed to write file for writing at: {}, byte {}", path, b);
            return KAZE_FALSE;
        }
    }

    // Catch leftovers
    if (b < size)
    {
        if ( !file.write((char *)(data) + b, size - b) )
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failure during file read call");
            return KAZE_FALSE;
        }
    }

    return KAZE_TRUE;
}

KAZE_NAMESPACE_END