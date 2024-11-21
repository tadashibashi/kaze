#include "BufferWriter.h"

#include <kaze/internal/core/debug.h>

KAZE_NS_BEGIN

BufferWriter::BufferWriter(const BufferWriterInit &initConfig) :
    m_mem(), m_head(), m_end(),
    m_arithmeticEndian(initConfig.arithmeticEndian),
    m_stringEndian(initConfig.stringEndian)
{
    if (initConfig.initialSize > 0)
    {
        m_mem = (Ubyte *)memory::alloc(initConfig.initialSize);
        if (m_mem)
        {
            m_head = m_mem;
            m_end = m_mem + initConfig.initialSize;
        }
    }
}

BufferWriter::~BufferWriter()
{
    if (m_mem)
    {
        memory::free(m_mem);
    }
}

auto BufferWriter::reserve(const Size newSize) -> Bool
{
    auto lastSize = size();
    if (lastSize < newSize)
    {
        auto newMem = memory::realloc(m_mem, newSize);
        if ( !newMem )
        {
            return KAZE_FALSE;
        }

        m_mem = static_cast<Ubyte *>(newMem);
        m_head = m_mem + lastSize;
        m_end = m_mem + newSize;
    }

    return KAZE_TRUE;
}

auto BufferWriter::writeString(const char *str, Size strLength, const BufferWriterStringOpts &opts) -> Size
{
    auto endian = (opts.endian == endian::Unknown) ? m_stringEndian : opts.endian;
    if (opts.fixedBufSize == std::numeric_limits<Size>::max())
    {
        // defer length to the string
        auto byteCount = writeRaw(str, strLength, endian == endian::Little);
        byteCount += writeFill(0, 1); // append null terminator
        return byteCount;
    }
    else
    {
        // use a fixed block size
        const auto blockSize = opts.fixedBufSize;
        if (blockSize == 0)
            return 0;

        strLength = strLength < blockSize ? strLength : blockSize;

        writeRaw(str, strLength, endian == endian::Little);
        if (blockSize > strLength)
        {
            writeFill(0, blockSize - strLength);
        }

        return blockSize;
    }
}

auto BufferWriter::writeFill(Ubyte value, Size byteCount) -> Size
{
    if (byteCount == 0) return 0;

    const auto targetSize = size() + byteCount;
    if (targetSize > maxSize())
    {
        if ( !reserve(targetSize * 2 + 1) )
        {
            return 0;
        }
    }

    memory::set(m_head, value, byteCount);

    m_head = m_mem + targetSize;
    return byteCount;
}

auto BufferWriter::writeRaw(const void *data, const Size byteCount,
                            const Bool reverse) -> Size
{
    if ( !data )
    {
        KAZE_PUSH_ERR(Error::NullArgErr, "required arg `data` was null");
        return 0;
    }

    if (byteCount == 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "can't write 0 bytes");
        return 0;
    }

    const auto targetSize = size() + byteCount;
    if (targetSize > maxSize())
    {
        if ( !reserve(targetSize * 2 + 1) )
        {
            return 0;
        }
    }

    if (reverse)
    {
        auto curSrc = (Ubyte *)data;
        for (auto curDest = m_head + byteCount - 1; curDest >= m_head; --curDest, ++curSrc)
        {
            *curDest = *curSrc;
        }
    }
    else
    {
        memory::copy(m_head, data, byteCount);
    }

    m_head += byteCount;
    return byteCount;
}

KAZE_NS_END
