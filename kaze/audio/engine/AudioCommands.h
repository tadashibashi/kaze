#pragma once

#include <kaze/audio/lib.h>
#include <kaze/audio/AudioTime.h>
#include <kaze/core/Handle.h>

KAUDIO_NS_BEGIN

class AudioBus;
class AudioContext;
class AudioEffect;
class AudioEngine;
class AudioSource;

namespace commands {

    // ===== Audio Context ====================================================

    struct ContextFlagRemovals
    {
        AudioContext *context;

        auto operator()() -> void;
    };


    // ===== Audio Effect =====================================================

    struct EffectSetParameter {
        /// Effect to set parameter on
        AudioEffect *effect;

        /// Parameter index
        Int paramIndex;

        /// Set parameter type
        enum class Type {
            Int,
            Float,
            String,
        };

        /// Value to set, use `type` as an indexer
        Variant<Int, Uint64, Float, String> value;

        auto operator()() -> void;
    };


    // ===== Audio Source =====================================================

    /// Set pause on AudioSource
    struct SourceSetPause
    {
        /// Source to set pause on
        AudioSource *source;

        /// Whether to release the source when pause occurs
        Bool releaseOnPause;

        /// Parent clock to set pause
        Uint64 clock;

        auto operator()() -> void;
    };

    /// Set unpause on AudioSource
    struct SourceSetUnpause
    {
        /// Source to unpause
        AudioSource *source;

        /// Parent clock to set unpause
        Uint64 clock;

        auto operator()() -> void;
    };

    /// Add an AudioEffect to an AudioSource
    struct SourceAddEffect {
        /// Source to add effect to
        AudioSource *source;

        /// Effect to add
        Handle<AudioEffect> effect;

        /// Effect index at which to insert
        Int insertIndex;

        auto operator()() -> void;
    };

    /// Remove an effect from an AudioSource
    struct SourceRemoveEffect {
        /// Source to remove effect from
        AudioSource *source;

        /// Effect to remove
        Handle<AudioEffect> effect;

        auto operator()() -> void;
    };

    /// Add fade point to AudioSource
    struct SourceAddFadePoint {
        /// Source to add fade point on
        AudioSource *source;

        /// Parent clock at which to set point
        Uint64 clock;

        /// Fade value to set at point
        Float value;

        auto operator()() -> void;
    };

    /// Remove fade points from an AudioSource
    struct SourceRemoveFadePoint {
        /// Source to remove fade point from
        AudioSource *source;

        /// Parent clock to start removing fade points at
        Uint64 beginClock;

        /// Parent clock to stop removing fade points at (not including it)
        Uint64 endClock;

        auto operator()() -> void;
    };

    struct SourceFadeTo {
        /// Source to perform fade-to on
        AudioSource *source;

        /// Destination clock
        Uint64 clock;

        /// Destination fade value
        Float value;

        auto operator()() -> void;
    };


    // ===== Audio Bus ========================================================

    /// Append an AudioSource to an AudioBus
    struct BusConnectSource {
        Handle<AudioBus> bus;
        Handle<AudioSource> source;

        auto operator()() -> void;
    };

    /// Remove an AudioSource from an AudioBus
    struct BusDisconnectSource {
        Handle<AudioBus> bus;
        Handle<AudioSource> source;

        auto operator()() -> void;
    };

    using AudioCommand = Variant<
        ContextFlagRemovals,
        EffectSetParameter,
        SourceSetPause,
        SourceSetUnpause,
        SourceAddEffect,
        SourceRemoveEffect,
        SourceAddFadePoint,
        SourceRemoveFadePoint,
        SourceFadeTo,
        BusConnectSource,
        BusDisconnectSource
    >;
};

using AudioCommand = commands::AudioCommand;

KAUDIO_NS_END
