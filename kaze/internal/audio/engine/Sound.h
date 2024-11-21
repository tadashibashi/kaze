#pragma once

#include <kaze/internal/audio/lib.h>
#include <kaze/internal/audio/AudioTime.h>
#include <kaze/internal/audio/AudioMarker.h>

#include <kaze/internal/core/Handle.h>
#include <kaze/internal/core/MemView.h>
#include <kaze/internal/core/ManagedMem.h>
#include <kaze/internal/core/MultiPool.h>

KAUDIO_NS_BEGIN

class AudioBus;
class AudioContext;
struct AudioSpec;
class AudioSource;

/// Description to instantiate a sound source
class Sound {
public:
    Sound();
    ~Sound();

    Sound(Sound &&other) noexcept;

    // Types of sounds to init,
    // - stream from disk
    //     o filename
    //     o target spec
    // - stream in-memory
    //     o filename
    //     o target spec
    //     o optional: in-memory pointer, persistance needed until stream freed as buffer is directly used
    enum InitFlags : Uint {
        None      = 0,
        Looping   = 1,
        OneShot   = 1 << 1,
        Stream    = 1 << 2, ///< Stream data (default streams from file)
        InMemory  = 1 << 3, ///< Store file data in memory, as opposed to streaming from file (forced true on web)
    };

    /// Add a marker into the Sound at a given position. Native units are in `TimeUnit::PCM`.
    /// \param[in] units time units of the `position` to place the marker at
    /// \param[in] position valueof the marker in `units` time units
    /// \param[in] label the text associated with the marker
    auto addMarker(Double position, AudioTime::Unit units, const String &label) -> Bool;

    /// Get a marker at index
    /// \param[in] index     index of the marker to look up; valid ranges are 0-markerCount. Marker count can be
    ///                  retrieved via `getMarkerCount`.
    /// \returns whether function succeeded; on `false` check `insound::popError()` for more details.
    auto getMarker(Size index) const -> AudioMarker;

    /// \returns the number of markers available in the Sound
    auto getMarkerCount() const -> Size;

    /// \returns the source Audio spec of the sound
    auto getSpec() const -> AudioSpec;

    /// \returns whether the sound is valid and ready to instantiate AudioSources
    [[nodiscard]]
    auto isOpen() const -> Bool;

    auto openFile(const String &filename, InitFlags flags, const AudioSpec &targetSpec) -> Bool;

    /// Open sound from const memory. Memory must be valid for the duration of usage by this class.
    /// \param[in]  mem        audio file memory to use
    /// \param[in]  flags      sound attributes
    /// \param[in]  targetSpec target sound spec, grab this from the Engine/Context.
    /// \returns `True` if open was successful.
    auto openConstMem(MemView<void> mem, InitFlags flags, const AudioSpec &targetSpec) -> Bool;

    /// Open sound from memory that is handed over to the Sound to manage.
    /// \param[in]  mem        audio file memory to use
    /// \param[in]  dealloc    deallocator for memory on `release`. If `Null`, memory::free will be used.
    /// \param[in]  flags      sound attributes
    /// \param[in]  targetSpec target sound spec, grab this from the Engine/Context.
    /// \returns `True` if open was successful.
    auto openMem(ManagedMem mem, InitFlags flags, const AudioSpec &targetSpec) -> Bool;

    auto init_() -> Bool;

    /// Release the sound resources. All instances of the sound should be released.
    /// TODO: Tracking mechanism that can call release on all sounds instantiated with Sound.
    ///       Each instance may need to hold a ref to the sound so that the system knows to release it.
    ///       A recursive master bus check could release all sounds with the Sound id.
    auto release_() -> void;
private:
    friend class AudioContext;
    friend class AudioEngine;

    /// Used by the Engine to instantiate a new source object. TODO: move elsewhere? maybe as a command?
    auto instantiate(AudioContext *context, Bool paused, Handle<AudioBus> bus, Handle<AudioSource> *outSource) -> Bool;

    struct Impl;
    Impl *m;
};

inline auto operator |(Sound::InitFlags a, Sound::InitFlags b) -> Sound::InitFlags
{
    return static_cast<Sound::InitFlags>(
        static_cast<Uint>(a) | static_cast<Uint>(b));
}

inline auto operator |=(Sound::InitFlags &a, Sound::InitFlags b) -> Sound::InitFlags &
{
    a = static_cast<Sound::InitFlags>(
        static_cast<Uint>(a) | static_cast<Uint>(b));
    return a;
}

KAUDIO_NS_END
