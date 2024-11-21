#pragma once
#include <kaze/internal/audio/lib.h>
#include <kaze/internal/audio/engine/AudioCommands.h>
#include <kaze/internal/audio/engine/AudioEffect.h>
#include <kaze/internal/audio/engine/AudioContext.h>
#include <kaze/internal/audio/engine/effects/PanEffect.h>
#include <kaze/internal/audio/engine/effects/VolumeEffect.h>
#include <kaze/internal/audio/FadePoint.h>
#include <kaze/internal/core/AlignedList.h>
#include <kaze/internal/core/Handle.h>

KAUDIO_NS_BEGIN

class AudioSource {
public:
    virtual ~AudioSource() = default;
    AudioSource() = default;
    AudioSource(AudioSource &&other) noexcept;

    /// Since Source is a poolable object, it must implement `init_`.
    /// All child classes must implement an `init_` function and call it's parent's `init_`. We don't enforce what
    /// the prototype is to allow for varying initialization signatures based on the need of each subclass.
    /// \param[in]  context      AudioContext to instantiate AudioSource with
    ///
    /// \param[in]  parentClock  Current parent clock
    ///
    /// \param[in]  paused       Whether to initialize the AudioSource in a paused state
    ///
    /// \returns whether initialization was successful.
    ///
    auto init_(AudioContext *context , Uint64 parentClock, Bool paused) -> Bool;

    /// Release for the user to call when no longer using this source.
    /// Make sure to call this when overriding.
    virtual auto release() -> void;

    /// Release callback that the MultiPool will automatically call
    virtual auto release_() -> void;

    /// Pause the sound source at a specific clock time
    /// \param[in]  clock            When to apply pause in parent clocks; use `getParentClock`
    ///                                  to get current parent clock;
    ///                              Value of `UINT64_MAX` executes pause right away;
    ///                              Set to `0` to clear the timed pause. You may also use `clearPauseTimer` for
    ///                                  clarity of intent;
    ///
    /// \param[in]  releaseOnPause   Whether to release the AudioSource when the pause occurs
    ///                              [optional, default: `False`]
    ///
    /// \note There is only one timed pause per AudioSource; it's separate from the unpause clock.
    ///
    /// \returns whether command was sent without error.
    ///
    auto pauseAt(Uint64 clock, Bool releaseOnPause = False) -> Bool;

    /// Clear pause timer
    /// \returns whether command was sent without error.
    auto clearTimedPause() -> Bool { return pauseAt(0); }

    /// Unpause the sound source at a specific clock time
    /// \param[in]  clock   When to apply unpause in parent clocks; use `getParentClock`
    ///                         to get current parent clock;
    ///                     Value `UINT64_MAX` executes unpause right away; set to 0 to clear timed unpause;
    ///                     You may also use `clearUnpauseTimer` for clarity of intent;
    ///
    /// \note There is only one timed unpause per AudioSource; it's separate from the unpause clock.
    ///
    /// \returns whether command was sent without error.
    ///
    auto unpauseAt(Uint64 clock) -> Bool;

    /// Clear unpause timer
    ///
    /// \returns whether command was sent without error.
    ///
    auto clearTimedUnpause() -> Bool { return unpauseAt(0); }

    /// Immediate pause or unpause
    /// \param[in]  pause   `true` - pause; `false` - unpause;
    ///
    /// \returns whether command was sent without error.
    ///
    auto setPaused(Bool pause) -> Bool;

    /// \returns whether the audio source is currently paused.
    [[nodiscard]]
    auto isPaused() const -> Bool;

    template <Poolable T, typename ...TArgs> requires std::is_base_of_v<AudioEffect, T>
    auto addEffect(const Int index, TArgs &&...args) -> Handle<T> //TODO: add this function to the Engine, so you don't need to access the lock guard or object pool
    {
        const auto effect = m_context->createObject(std::forward<TArgs>(args)...);
        if ( !effect.isValid() )
        {
            return {};
        }

        m_context->pushCommand(commands::SourceAddEffect {
            .source = this,
            .effect = static_cast<Handle<AudioEffect>>(effect),
            .insertIndex = index,
        });

        return effect;
    }

    auto removeEffect(const Handle<AudioEffect> &effect) -> Bool;

    [[nodiscard]]
    auto getEffect(Size position) -> Handle<AudioEffect>;

    [[nodiscard]]
    auto getEffect(Size position) const -> Handle<const AudioEffect>;

    [[nodiscard]]
    auto getEffectCount() const -> Size;

    [[nodiscard]]
    auto getClock() const -> Uint64;

    [[nodiscard]]
    auto getParentClock() const -> Uint64;

    [[nodiscard]]
    auto getPannerEffect() -> Handle<PanEffect> { return m_panner; }
    [[nodiscard]]
    auto getPannerEffect() const -> Handle<const PanEffect>
    {
        return static_cast< Handle<const PanEffect> >(m_panner);
    }

    [[nodiscard]]
    auto getVolume() const -> Float;
    auto setVolume(Float value) -> void;

    [[nodiscard]]
    auto getVolumeEffect() -> Handle<VolumeEffect> { return m_volume; }
    [[nodiscard]]
    auto getVolumeEffect() const -> Handle<const VolumeEffect>
    {
        return static_cast< Handle<const VolumeEffect> >(m_volume);
    }

    auto addFadePoint(Uint64 clock, Float value) -> Bool;

    auto fadeTo(Uint64 clock, Float value) -> Bool;

    auto removeFadePoints(Uint64 clockBeginPoint, Uint64 clockEndPoint) -> Bool;

    [[nodiscard]]
    auto getFadeValue() const -> Float;

    /// Whether this AudioSource is marked for discard, i.e. release was called.
    auto shouldDiscard() const -> Bool { return m_shouldDiscard; }

    auto read(const Ubyte **pcmPtr, Int64 length) -> Int64;
protected:
    [[nodiscard]]
    auto context() -> AudioContext * { return m_context; }
    [[nodiscard]]
    auto context() const -> const AudioContext * { return m_context; }

private:
    friend class AudioContext;
    friend class AudioBus;

    /// VIRTUAL: Required
    /// Implementation for retrieving PCM data from this AudioSource
    /// \param[in]  output  Pointer to the buffer to fill
    /// \param[in]  length  Size to read into the buffer in bytes
    /// \returns the number of bytes actually read, `-1` on error.
    virtual auto readImpl(Ubyte *output, Int64 length) -> Int64 = 0;

    /// VIRTUAL: Optional
    /// The engine calls this in the mixer thread to update clock values
    /// (called recursively from master bus)
    /// \param[in]  parentClock  parent clock value to set
    /// \returns whether set was successful.
    virtual auto updateParentClock(Uint64 parentClock) -> Bool;

    // ----- Private implementation -------------------------------------------

    auto swapBuffers(AlignedList<Ubyte , 16> *buffer) -> void;

    // ----- Commands ---------------------------------------------------------
    friend struct commands::SourceSetPause;
    friend struct commands::SourceSetUnpause;
    auto setPauseImpl(Bool pause, Uint64 clock, Bool releaseOnPause) -> void;

    friend struct commands::SourceAddEffect;
    auto addEffectImpl(Int insertIndex, const Handle<AudioEffect> &effect) -> void;

    friend struct commands::SourceRemoveEffect;
    auto removeEffectImpl(const Handle<AudioEffect> &effect) -> void;

    friend struct commands::SourceAddFadePoint;
    auto addFadePointImpl(Uint64 clock, Float value) -> void;

    friend struct commands::SourceRemoveFadePoint;
    auto removeFadePointImpl(Uint64 beginClock, Uint64 endClock) -> void;

    friend struct commands::SourceFadeTo;
    auto fadeToImpl(Uint clock, Float value) -> void;

    // ----- Data members -----------------------------------------------------
    /// Cached ref to the engine
    AudioContext *m_context{};

    /// Attached audio effects
    List< Handle<AudioEffect> > m_effects{};

    Handle<VolumeEffect> m_volume{};
    Handle<PanEffect> m_panner{};

    /// Temporary audio buffers
    AlignedList<Ubyte, 16> m_outBuffer{}, m_inBuffer{};

    /// Current fade points
    List<FadePoint> m_fadePoints{};

    // Core State
    Float m_fadeValue{1.f};
    Uint64 m_clock{}, m_parentClock{};
    Bool m_paused{};
    Uint64 m_pauseClock{std::numeric_limits<Uint64>::max()}, m_unpauseClock{std::numeric_limits<Uint64>::max()};
    Bool m_releaseOnPauseClock{};
    Bool m_shouldDiscard{};
};

KAUDIO_NS_END
