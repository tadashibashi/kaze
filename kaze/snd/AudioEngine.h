#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioCommands.h>
#include <kaze/snd/AudioContext.h>
#include <kaze/snd/AudioDevice.h>
#include <kaze/snd/AudioEffect.h>
#include <kaze/snd/Sound.h>

#include <kaze/core/Handle.h>
#include <kaze/core/ManagedMem.h>
#include <kaze/core/MultiPool.h>
#include <kaze/core/MemView.h>

KSND_NS_BEGIN

struct AudioSpec;
class Bus;
class PCMSource;
class SoundBuffer;
class Source;
class StreamSource;

struct AudioEngineInit {
    Int samplerate;
    Int bufferFrameSize;
};

class AudioEngine {
public:
    AudioEngine();
    ~AudioEngine();

    auto open(const AudioEngineInit &config) -> Bool;
    auto close() -> void;

    /// Whether engine is currently open from a prior call to `AudioEngine::open`
    [[nodiscard]]
    auto isOpen() const -> Bool;

    /// Create a sound from a file.
    /// \param[in] filepath path to the file to open (.WAV, .FLAC, .MP3, .OGG, etc.)
    ///
    /// \param[in] flags    attributes to open the sound with
    ///
    /// \returns Sound object, or an invalid handle on error.
    [[nodiscard]]
    auto createSound(const String &filepath, Sound::InitFlags flags) -> Handle<Sound>;

    /// Create a sound from a file already loaded into memory.
    /// \param[in] mem            Contains memory pointing to a contiguous file data buffer;
    ///                               it must contain a valid .WAV, .FLAC, .MP3, .OGG, file
    /// \param[in] flags          Attributes to open the sound with.
    ///
    /// \returns Sound object, or an invalid handle on error.
    [[nodiscard]]
    auto createSound(MemView<void> mem, Sound::InitFlags flags) -> Handle<Sound>;

    /// Create a sound from a file already loaded into memory, passing responsibility for the memory
    /// to the Sound object.
    /// \param[in]  mem       Contains memory with deallocator, pointing to a contiguous file data buffer;
    ///                           it must contain a valid .WAV, .FLAC, .MP3, .OGG, file.
    /// \param[in]  flags     Attributes to open the sound with.
    ///
    /// \returns Sound object, or an invalid handle on error.
    [[nodiscard]]
    auto createSound(const ManagedMem &mem, Sound::InitFlags flags) -> Handle<Sound>;

    /// Release a created sound.
    /// \param[in] sound       sound to release
    auto releaseSound(const Handle<Sound> &sound) -> void;

    /// Instantiate and play a sound source
    /// \param[in] sound     Sound object to play. It must have been created via `createSound` from
    ///                          this `AudioEngine` object.
    /// \param[in] paused    Whether to start the sound in a paused state.
    ///
    /// \param[in] bus       Bus to output this sound to, use `{}` to indicate null, which
    ///                          defaults to the master bus.
    /// \returns AudioSource sound instance, or an invalid handle on error.
    auto playSound(const Handle<Sound> &sound, Bool paused = False, const Handle<AudioBus> &bus = {}) -> Handle<AudioSource>;

    /// Create a new bus to use in the mixing graph
    /// \param paused whether bus should start off paused on initialization
    /// \param output output bus to feed this bus to [optional, default: master bus]
    ///
    /// \returns AudioBus instance, or an invalid handle on error.
    [[nodiscard]]
    auto createBus(Bool paused, const Handle<AudioBus> &output = {}) -> Handle<AudioBus>;

    /// Retrieve the engine's device ID. If zero, the audio device is uninitialized.
    [[nodiscard]]
    auto getDeviceID() const -> Uint;

    /// Get audio spec
    [[nodiscard]]
    auto getSpec() const -> const AudioSpec &;

    /// Get the size of audio buffer in bytes
    [[nodiscard]]
    auto getBufferSize() const -> Uint;

    /// Get the master bus
    [[nodiscard]]
    auto getMasterBus() const -> Handle<AudioBus>;

    /// Pause the audio device
    auto setPaused(Bool value) -> void;

    /// \returns whether the device is currently paused
    [[nodiscard]]
    auto getPaused() const -> Bool;

    /// Call this once per game frame ~30-60fps
    auto update() -> void;

private:
    struct Impl;
    Impl *m;
};

KSND_NS_END
