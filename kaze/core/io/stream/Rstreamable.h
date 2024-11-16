#pragma once
#include <kaze/core/lib.h>

#include "SeekBase.h"

KAZE_NS_BEGIN

class Rstreamable {
public:
    virtual ~Rstreamable() = default;

    /// Open a file for streaming from a filesystem path
    /// \param[in]  path   path to the file to open
    /// \returns whether operation was successful
    virtual auto openFile(const String &path) -> Bool = 0;

    /// Close the stream; safe to call, even if already closed.
    virtual auto close() -> void = 0;

    /// \returns whether file stream is currently opened
    [[nodiscard]]
    virtual auto isOpen() const -> Bool = 0;

    /// \returns the size of the stream, if available.
    /// \returns -1 if the stream size is not determinable.
    [[nodiscard]]
    virtual auto size() const -> Int64 = 0;

    /// \returns the current read position in the stream relative to the start point.
    [[nodiscard]]
    virtual auto tell() const -> Int64 = 0;

    /// \returns whether the end-of-file flag has been raised. Occurs when user has read
    ///          past the end of the file. If a read reaches the end, but not past the
    ///          last byte, this flag will not be set until the next read, granted
    ///          the user did not seek to another position.
    [[nodiscard]]
    virtual auto isEof() const -> Bool = 0;

    /// Copy bytes from the stream into a buffer
    /// \param[out]  buffer  pointer to write to
    /// \param[in]   bytes   number of bytes to read from the stream
    /// \returns the number of bytes read. If less than `bytes`, end-of-file likely occured.
    /// \returns `-1` if an error occurred.
    virtual auto read(void *buffer, Int64 bytes) -> Int64 = 0;

    /// Seek to a position in the file stream
    /// \param[in]   position  byte position
    /// \param[in]   base      relative location to count the position from [optional, default: Begin]
    /// \note certain stream bases may not be available depending on the type of Rstreamable.
    ///       Pay attention to the docs and use common sense when setting.
    /// \returns whether operation was successful.
    virtual auto seek(Int64 position, SeekBase base = SeekBase::Begin) -> Bool = 0;
};

KAZE_NS_END
