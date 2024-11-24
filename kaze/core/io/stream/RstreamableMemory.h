#pragma once
#include <kaze/core/lib.h>
#include <kaze/core/traits.h>
#include <kaze/core/ManagedMem.h>
#include <kaze/core/MemView.h>

#include "Rstreamable.h"

KAZE_NS_BEGIN

class RstreamableMemory : public Rstreamable {
public:
    RstreamableMemory() = default;
    ~RstreamableMemory() override = default;

    /// Open a file for streaming from a filesystem path
    /// \param[in]  path   path to the file to open
    /// \returns whether operation was successful
    auto openFile(const String &path) -> Bool override;

    auto openMem(ManagedMem mem) -> Bool;

    auto openConstMem(MemView<void> mem) -> Bool;

    /// Close the stream; safe to call, even if already closed.
    auto close() -> void override;

    /// \returns whether file stream is currently opened
    [[nodiscard]]
    auto isOpen() const -> Bool override;

    /// \returns the size of the stream, if available.
    /// \returns -1 if the stream size is not determinable.
    [[nodiscard]]
    auto size() const -> Int64 override;

    /// \returns the current read position in the stream relative to the start point.
    [[nodiscard]]
    auto tell() const -> Int64 override;

    /// \returns whether the end-of-file flag has been raised. Occurs when user has read
    ///          past the end of the file. If a read reaches the end, but not past the
    ///          last byte, this flag will not be set until the next read, granted
    ///          the user did not seek to another position.
    [[nodiscard]]
    auto isEof() const -> Bool override;

    /// Copy bytes from the stream into a buffer
    /// \param[out]  buffer  pointer to write to
    /// \param[in]   bytes   number of bytes to read from the stream
    /// \returns the number of bytes read. If less than `bytes`, end-of-file likely occured.
    /// \returns `-1` if an error occurred.
    auto read(void *buffer, Int64 bytes) -> Int64 override;

    /// Seek to a position in the file stream
    /// \param[in]   position  byte position
    /// \param[in]   base      relative location to count the position from [optional, default: Begin]
    /// \note certain stream bases may not be available depending on the type of Rstreamable.
    ///       Pay attention to the docs and use common sense when setting.
    /// \returns whether operation was successful.
    auto seek(Int64 position, SeekBase base = SeekBase::Begin) -> Bool override;

    auto data() const noexcept -> const Ubyte * { return m_data; }
private:
    auto cleanupData() -> void;
    Ubyte *m_data{}, *m_head{}, *m_end{};
    Bool m_eof{};

    funcptr_t<void(void *, void *)> m_deallocator{};
    void *m_userptr;
};

KAZE_NS_END
