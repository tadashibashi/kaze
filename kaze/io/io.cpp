#include "io.h"
#include <kaze/debug.h>
#include <kaze/memory.h>

#include <bgfx/bgfx.h>
#include <filesystem>
#include <fstream>

KAZE_NAMESPACE_BEGIN
    constexpr int BytesPerRead = 1024;
static auto loadFile(std::ifstream &file, Ubyte *data, Size size) -> Bool
{
    if ( !data )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `outData` was null");
        return KAZE_FALSE;
    }

    if ( !size )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `outSize` was 0");
        return KAZE_FALSE;
    }

    if ( !file.is_open() )
    {
        KAZE_CORE_ERRCODE(Error::FileOpenErr, "std::ifstream provided was not open");
        return KAZE_FALSE;
    }

    Size b = 0;
    for (const Int64 limit = static_cast<Int64>(size) - static_cast<Int64>(BytesPerRead); b <= limit; b += BytesPerRead)
    {
        if ( !file.read((char *)data + b, BytesPerRead) )
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failure during file read call");
            kaze::release(data);
            return KAZE_FALSE;
        }
    }

    // Catch leftovers
    if (b < size)
    {
        if ( !file.read((char *)data + b, size - b) )
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failure during file read call");
            kaze::release(data);
            return KAZE_FALSE;
        }
    }

    return KAZE_TRUE;
}

auto loadFile(StringView path, Ubyte **outData, Size *outSize) -> Bool
{
    if ( !path.data() )
    {
        KAZE_CORE_ERRCODE(Error::NullArgErr, "Required param `path.data()` was null");
        return KAZE_FALSE;
    }

    std::ifstream file(path, std::ios::in | std::ios::binary);
    if ( !file.is_open() )
    {
        KAZE_CORE_ERRCODE(Error::FileOpenErr, "failed to open file at: \"{}\"", path);
        return KAZE_FALSE;
    }

    // get the file size
    Size byteLength = 0;
    if ( !file.seekg(0, std::ios::end) )
    {
        KAZE_CORE_ERRCODE(Error::FileSeekErr, "Failed to seek to end of file");
        return KAZE_FALSE;
    }

    byteLength = file.tellg();
    if ( !file.seekg(0, std::ios::beg) ) // seek back to beginning to read entire file
    {
        KAZE_CORE_ERRCODE(Error::FileSeekErr, "Failed to seek to beginning of file");
        return KAZE_FALSE;
    }

    if (byteLength == 0)
    {
        KAZE_CORE_ERRCODE(Error::RuntimeErr, "Empty file: file must have more than 0 bytes");
        return KAZE_FALSE;
    }

    auto data = kaze::alloc<Ubyte>(byteLength);
    if ( !data )
    {
        return KAZE_FALSE;
    }

    if ( !loadFile(file, data, byteLength) )
    {
        kaze::release(data);
        return KAZE_FALSE;
    }

    *outData = data;
    *outSize = byteLength;
    return KAZE_TRUE;
}

auto writeFile(StringView path, const Memory mem) -> Bool
{
    const auto size = mem.size();
    const auto data = (const char *)mem.data();

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
        if ( !file.write(data + b, BytesPerRead) )
        {
            KAZE_CORE_ERRCODE(Error::FileWriteErr, "Failed to write file for writing at: {}, byte {}", path, b);
            return KAZE_FALSE;
        }
    }

    // Catch leftovers
    if (b < size)
    {
        if ( !file.write(data + b, size - b) )
        {
            KAZE_CORE_ERRCODE(Error::FileReadErr, "Failure during file read call");
            return KAZE_FALSE;
        }
    }

    return KAZE_TRUE;
}

KAZE_NAMESPACE_END
