#include "RstreamableFile.h"

KAZE_NS_BEGIN

RstreamableFile::RstreamableFile() : m_file()
{

}

auto RstreamableFile::openFile(const String &path) -> Bool
{
    std::ifstream file(path, std::ios::binary | std::ios::in);
    if ( !file.is_open() )
    {
        KAZE_PUSH_ERR(Error::FileOpenErr, "Failed to open file: {}", path);
        return False;
    }

    m_file = std::move(file);
    return True;
}

auto RstreamableFile::close() -> void
{
    m_file.close();
}

auto RstreamableFile::isOpen() const -> Bool
{
    return m_file.is_open();
}

auto RstreamableFile::size() const -> Int64
{
    const auto lastPos = m_file.tellg();

    m_file.seekg(0, std::ios::end);
    const auto sizePos = m_file.tellg();

    m_file.seekg(lastPos, std::ios::beg);
    return static_cast<Int64>(sizePos);
}

auto RstreamableFile::tell() const -> Int64
{
    return static_cast<Int64>(m_file.tellg());
}

auto RstreamableFile::isEof() const -> Bool
{
    return m_file.eof();
}

auto RstreamableFile::read(void *buffer, Int64 bytes) -> Int64
{
    if ( !m_file.is_open() )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "std::ifstream::read attempted on unopened file");
        return -1LL;
    }

    if (m_file.eof())
    {
        return 0;
    }

    if (m_file.fail())
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "std::ifstream::read attempted in bad state");
        return -1LL;
    }

    if (bytes == 0)
        return 0;

    if ( !buffer ) // without a user-provided buffer, just perform a seek
    {
        const auto lastPos = m_file.tellg();
        m_file.seekg(bytes, std::ios::cur);
        return m_file.tellg() - lastPos;
    }

    m_file.read((char *)buffer, static_cast<std::streamsize>(bytes));

    if (m_file.fail() && !m_file.eof())
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Fail bit set after reading file");
        return -1;
    }

    return m_file.gcount();
}

static auto getIosBase(SeekBase base) -> std::ios_base::seekdir
{
    switch(base)
    {
    case SeekBase::Begin:   return std::ios::beg;
    case SeekBase::Current: return std::ios::cur;
    case SeekBase::End:     return std::ios::end;
    default:
        KAZE_CORE_WARN("Invalid SeekBase value passed, fallback to std::ios::beg");
        return std::ios::beg;
    }
}

auto RstreamableFile::seek(Int64 position, SeekBase base) -> Bool
{
    if ( !m_file.is_open() )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "std::ifstream::seekg attempted on unopened file");
        return False;
    }

    m_file.clear();
    m_file.seekg(static_cast<std::streamoff>(position), getIosBase(base));

    if (m_file.fail())
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failure during std::ifstream::seekg");
        return False;
    }

    return True;
}

KAZE_NS_END
