#pragma once

#include <kaze/audio/lib.h>
#include <kaze/audio/AudioSpec.h>
#include <kaze/core/MemView.h>

KAUDIO_NS_BEGIN

class SoundBuffer {
public:
    SoundBuffer();
    SoundBuffer(const String &filepath, const AudioSpec &targetSpec);
    ~SoundBuffer();

    SoundBuffer(SoundBuffer &&other) noexcept;
    SoundBuffer &operator=(SoundBuffer &&other) noexcept;

    /// Load sound and convert it to the target specification
    /// \param[in] filepath    path to the sound file
    /// \param[in] targetSpec  the specification to convert this buffer to on load
    /// \returns whether load succeeded.
    auto load(const String &filepath, const AudioSpec &targetSpec) -> Bool;

    /// Load sound and convert it to the target specification
    /// \param[in]  mem        in-memory sound file data
    /// \param[in]  targetSpec the specification to convert this buffer to on load
    /// \returns whether load succeeded.
    auto load(const MemView<void> mem, const AudioSpec &targetSpec) -> Bool;

    /// Free sound buffer resources
    auto unload() -> void ;

    /// \returns if sound is currently loaded with data
    [[nodiscard]]
    auto isLoaded() -> bool  const { return m_buffer.load() != nullptr; }

    /// \returns the size of buffer in bytes
    [[nodiscard]]
    auto size() const { return m_bufferSize; }

    /// \returns the data buffer. If not loaded, `nullptr` will be returned.
    [[nodiscard]]
    auto data() -> Ubyte * { return m_buffer.load(); }

    /// \returns the data buffer. If not loaded, `nullptr` will be returned.
    [[nodiscard]]
    auto data() const -> const Ubyte * { return m_buffer.load(); }

    /// \returns spec of the sound buffer data
    [[nodiscard]]
    auto spec() const -> const AudioSpec & { return m_spec; }

    /// Replace current buffer with a new one
    /// \param[in]  mem    Memory of parsed sound data
    /// \param[in]  spec   Audio specification of the data
    auto emplace(MemView <void> mem, const AudioSpec &spec) -> void;
private:
    Size  m_bufferSize;
    std::atomic<Ubyte *> m_buffer;
    AudioSpec m_spec;
};

KAUDIO_NS_END
