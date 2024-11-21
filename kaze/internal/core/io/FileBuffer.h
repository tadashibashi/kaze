#pragma once

#include <kaze/internal/core/lib.h>

KAZE_NS_BEGIN
    /// Handles file loading into memory. Loads it all at once instead of a streaming interface
class FileBuffer {
public:
    FileBuffer();
    /// Create and open file in one go
    /// \param[in] filepath path to the file to open
    explicit FileBuffer(StringView filepath);
    ~FileBuffer();

    // copying copies the entire internal data buffer
    FileBuffer(const FileBuffer &other);
    auto operator=(const FileBuffer &other) -> FileBuffer &;

    // moving moves the internal data buffer
    FileBuffer(FileBuffer &&other) noexcept;
    auto operator=(FileBuffer &&other) noexcept -> FileBuffer &;

    /// Load binary data from a file
    /// \param[in] filepath   path to the file to open
    /// \returns whether operation was succeessful
    auto open(StringView filepath) -> Bool;

    /// Release file resources, called automatically on destruction
    auto close() -> void;

    /// Check whether internal data buffer is currently loaded from a file
    auto isOpen() const noexcept -> Bool { return m_data != nullptr; }

    /// Internal data buffer; if not open, it will be `nullptr`
    auto data() const noexcept -> const Ubyte * { return m_data; }

    /// Byte size of internal data buffer
    auto size() const noexcept -> Size { return m_dataSize; }

  private:
    Ubyte *m_data;   ///< internal data buffer
    Size m_dataSize; ///< length of the data in the buffer
};

KAZE_NS_END
