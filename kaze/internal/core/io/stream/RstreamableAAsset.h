#pragma once
#include <kaze/internal/core/platform/defines.h>

#include "Rstreamable.h"

class AAsset;

KAZE_NS_BEGIN

class RstreamableAAsset : public Rstreamable {
public:
    RstreamableAAsset();
    ~RstreamableAAsset() override;

    auto openFile(const String &path) -> Bool override;
    auto openFile(const String &path, bool inMemory) -> Bool;

    auto close() -> void override;

    [[nodiscard]]
    auto isOpen() const -> Bool override;

    [[nodiscard]]
    auto isEof() const -> Bool override;

    [[nodiscard]]
    auto size() const -> Int64 override;

    [[nodiscard]]
    auto tell() const -> Int64 override;

    auto read(void *buffer, Int64 bytes) -> Int64 override;

    auto seek(Int64 position, SeekBase base = SeekBase::Begin) -> Bool override;

private:
    auto cleanupAsset() -> void;

    AAsset *m_asset;
    Int64 m_pos, m_size;
    Bool m_eof;
};

KAZE_NS_END
