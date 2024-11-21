#include "RstreamableAAsset.h"
#include <cstdio>

#if !KAZE_PLATFORM_ANDROID
#   error RstreamableAAsset is only available on Android
#endif

#include <kaze/internal/core/platform/native/android/AndroidNative.h>
#include <android/asset_manager.h>

KAZE_NS_BEGIN

RstreamableAAsset::RstreamableAAsset() : m_asset(), m_pos(), m_size(), m_eof()
{

}

RstreamableAAsset::~RstreamableAAsset()
{
    close();
}

auto RstreamableAAsset::cleanupAsset() -> void
{
    if (m_asset)
    {
        android::closeAsset(m_asset);
    }
}

auto RstreamableAAsset::openFile(const String &path) -> Bool
{
    return openFile(path, false);
}

auto RstreamableAAsset::openFile(const String &path, bool inMemory) -> Bool
{
    AAsset *asset = inMemory ?
        android::openAsset(path.c_str()) :
        android::openAssetStream(path.c_str());

    if ( !asset )
    {
        return False;
    }

    const auto length = AAsset_getLength64(asset);

    cleanupAsset();
    m_asset = asset;
    m_eof = False;
    m_pos = 0;
    m_size = static_cast<Int64>(length);

    return True;
}

auto RstreamableAAsset::close() -> void
{
    cleanupAsset();
    m_asset = Null;
    m_pos = 0;
    m_size = 0;
    m_eof = False;
}

auto RstreamableAAsset::isOpen() const -> Bool
{
    return m_asset != Null;
}

auto RstreamableAAsset::isEof() const -> Bool
{
    return m_eof;
}

auto RstreamableAAsset::size() const -> Int64
{
    return m_size;
}

auto RstreamableAAsset::tell() const -> Int64
{
    return m_pos;
}

auto RstreamableAAsset::read(void *buffer, Int64 bytes) -> Int64
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Attempted to RstreamableAAsset::read on unopened asset");
        return -1LL;
    }

    // Eof checks
    if (m_eof)
    {
        return 0;
    }

    if (m_pos >= m_size)
    {
        m_eof = true;
        return 0;
    }

    // Do read
    const auto count = AAsset_read(m_asset, buffer, bytes);
    if (count < 0)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Error occurred during AAsset_read");
        return -1LL;
    }
    else if (count < bytes)
    {
        m_eof = true;
    }

    // Done, commit results
    m_pos += count;
    return count;
}

auto RstreamableAAsset::seek(Int64 position, SeekBase base) -> Bool
{
    if ( !isOpen() )
    {
        KAZE_PUSH_ERR(Error::LogicErr, "Attempted to RstreamableAAsset::seek on unopened asset");
        return False;
    }

    Int64 finalPosition;
    switch(base)
    {
    case SeekBase::Begin: finalPosition = 0; break;
    case SeekBase::Current: finalPosition = m_pos; break;
    case SeekBase::End: finalPosition = m_size; break;
    default:
        KAZE_CORE_WARN("Invliad SeekBase enum passed to RstreamableAAsset::seek; "
            "falling back to Begin");
        finalPosition = 0;
        break;
    }

    finalPosition += position;
    if (finalPosition > m_size || finalPosition < 0)
    {
        KAZE_PUSH_ERR(Error::InvalidArgErr, "RstreamableAAsset::seek position is out of range. "
            "Must be `pos >= 0 && pos <= {}`, but got `{}`", m_size, position);
        return False;
    }

    const auto result = AAsset_seek64(m_asset, finalPosition, SEEK_SET);
    if (result < 0)
    {
        KAZE_PUSH_ERR(Error::RuntimeErr, "Failure during AAsset_seek64");
        return False;
    }

    m_pos = finalPosition;
    return True;
}

KAZE_NS_END
