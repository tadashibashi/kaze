#pragma once
#include <kaze/internal/audio/lib.h>
#include <kaze/internal/audio/engine/AudioSource.h>

#include <kaze/internal/core/ManagedMem.h>
#include <kaze/internal/core/io/stream/SeekBase.h>
#include <kaze/internal/core/MemView.h>

KAUDIO_NS_BEGIN

/// StreamSource Initialization struct used in StreamSource::_init
struct StreamSourceInit {
    /// Audio context object
    AudioContext *context;

    /// Path to file, or in-memory
    Variant<ManagedMem, MemView<void>, String> pathOrMemory;

    /// Initial parent clock value
    Uint64 parentClock;

    /// Whether stream should start paused
    Bool paused;

    /// Whether stream should loop
    Bool isLooping;

    /// Whether stream is a oneshot (auto-release on end)
    Bool isOneShot;

    /// Whether to load file into memory and stream from RAM [optional, default: `False`]
    /// \note Only relevant if you pass a filename String to `pathOrMemory`.
    Bool inMemory = False;
};

class StreamSource final : public AudioSource {
public:
    StreamSource();
    ~StreamSource() override;
    StreamSource(StreamSource &&other) noexcept;

    // Pool init/release
    auto init_(const StreamSourceInit &config) -> Bool;
    auto release_() -> void override;

    // Open stream from a file
    auto openFile(const String &filepath, Bool inMemory = false) -> Bool;

    /// Open stram from const memory. Memory pointer must not be moved or invalidated for
    /// the duration that this source is used.
    auto openConstMem(MemView<void> mem) -> Bool;
    auto openMem(ManagedMem mem) -> Bool;

    [[nodiscard]]
    auto isOpen() const -> Bool;

    auto setLooping(Bool looping) -> Bool;
    auto getLooping() const -> Bool;

    [[nodiscard]]
    auto getPosition(AudioTime::Unit units) const -> Double;
    auto setPosition(AudioTime::Unit units, Uint64 position, SeekBase base) -> Bool;

private:
    auto readImpl(Ubyte *output, Int64 length) -> Int64 override;
    struct Impl;
    Impl *m;
};

KAUDIO_NS_END
