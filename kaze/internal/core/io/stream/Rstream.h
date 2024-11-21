#pragma once
#include <kaze/internal/core/lib.h>
#include <kaze/internal/core/ManagedMem.h>
#include <kaze/internal/core/MemView.h>

#include "SeekBase.h"

KAZE_NS_BEGIN

class Rstreamable;

/// Wrapper around Rstreamables that handles platform-specifics and memory management
/// in an RAII container
class Rstream {
public:
    Rstream() = default;
    ~Rstream();

    Rstream(Rstream &&other) noexcept;
    auto operator= (Rstream &&other) noexcept -> Rstream &;

    /// Open a file for streaming
    /// \param[in]  path      System file path to open
    /// \param[in]  inMemory  pass `true` to open contents at once in memory;
    ///                       pass `false` to stream contents from disk
    /// \returns whether operation was successful.
    auto openFile(const String &path, Bool inMemory = False) -> Bool;

    /// Pass memory for streaming, it needs to remain valid and immutable for the duration of the
    /// time this class makes use of it
    /// \param[in]  mem  memory to stream
    /// \returns whether operation was successful.
    auto openConstMem(MemView<void> mem) -> Bool;

    /// Pass memory for streaming; it also hands over responsibility of memory to this Rstream
    /// \param[in]  mem          memory to stream
    /// \param[in]  deallocator  callback to deallocate memory when close is called or Rstream is destroyed;
    ///                          [optional, default: `memory::free`]
    /// \returns whether operation was successful.
    auto openMem(ManagedMem mem) -> Bool;

    /// Close the stream, invalidating it.
    auto close() -> void;

    /// \returns Whether stream is currently open.
    [[nodiscard]]
    auto isOpen() const -> Bool;

    /// \returns Number of bytes in the stream, or `-1` on error, or if size cannot be determined.
    [[nodiscard]]
    auto size() const -> Int64;

    /// \returns Current read position in the buffer in bytes offset from the start.
    [[nodiscard]]
    auto tell() const -> Int64;

    /// \returns Whether the end-of-file flag has been set.
    [[nodiscard]]
    auto isEof() const -> Bool;

    /// Read a number of bytes from the stream
    /// \param[out]  buffer   pointer to write data to
    /// \param[in]   bytes    number of bytes to read
    /// \returns number of bytes read, or -1 if an error occurred.
    auto read(void *buffer, Int64 bytes) -> Int64;

    /// Seek to a position in the buffer
    /// \param[in] position   byte position to move to relative to `base`
    /// \param[in] base       base to seek from
    /// \returns whether seek succeeded.
    auto seek(Int64 position, SeekBase base = SeekBase::Begin) -> Bool;

    [[nodiscard]]
    auto stream() -> Rstreamable * { return m_stream; }
    [[nodiscard]]
    auto stream() const -> const Rstreamable * { return m_stream; }
private:
    Rstreamable *m_stream{};
};

KAZE_NS_END
