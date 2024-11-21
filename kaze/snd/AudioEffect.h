#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioCommands.h>
#include <kaze/snd/AudioContext.h>

KSND_NS_BEGIN

/// Base class for an audio effect, which is insertable into any Source object
class AudioEffect {
public:
    /// Contains a parameter value. (Simple wrapper around Variant)
    struct Param {
    public:
        using Type = Variant<KAZE_NS::Int, KAZE_NS::Uint64, KAZE_NS::Float, KAZE_NS::String>;
        explicit Param(const Type &value) : m_value(value) { }

        enum Enum : Size {
            Int,
            Uint64,
            Float,
            String,
        };

        /// \returns the type enum index of this value
        auto type() const noexcept -> Enum
        {
            return static_cast<Enum>(m_value.index());
        }

        /// Get the value from this param.
        /// Check for the type via `Param::index()`.
        /// \throws std::bad_variant_access on bad cast if type does not align.
        template <typename T>
        auto get() const -> const T &
        {
            return std::get<T>(m_value);
        }

        auto getVariant() const -> const Type & { return m_value; }
    private:
        Type m_value;
    };

    AudioEffect() : m_context() { }
    AudioEffect(AudioEffect &&other) noexcept;
    virtual ~AudioEffect() = default;

    /// VIRTUAL: Optional
    /// Pool clean up logic
    virtual void release_() { }

    /// Set a parameter value (gets sent to the command queue, so it won't appear until next frame)
    /// \param[in]  index   Parameter index
    /// \param[in]  value   Must be either Int, Uint64, Float, or String.
    /// \returns whether command was sent successfully.
    auto sendParam(Int index, Param::Type value) -> Bool;

protected:
    [[nodiscard]]
    auto context() -> AudioContext * { return m_context; }
    [[nodiscard]]
    auto context() const -> const AudioContext * { return m_context; }

private:
    friend class AudioEngine;
    friend class AudioSource;
    friend class commands::EffectSetParameter;

    /// VIRTUAL: Optional
    /// Override this if you need to process parameter sets.
    /// Don't set member parameters directly as it may occur during audio thread.
    /// Instead, set via `sendParam` function and apply changes in `receiveParam` function.
    /// \param[in] index index passed in `sendFloat` now received
    /// \param[in] param value passed in `sendFloat` now received
    virtual auto receiveParam(Int index, const Param &param) -> void {}

    /// VIRTUAL: Required
    /// Required to override for this effect's processing logic
    /// \param[in] input  input buffer filled with data to process
    /// \param[in] output output buffer to write to (comes cleared to 0)
    /// \param[in] count  number of samples, the length of both input and output arrays. This value is guaranteed to
    ///               be a multiple of 4 for optimization purposes.
    /// \note both input and output buffers are interleaved stereo, so samples will occur in L-R-L-R order
    /// \returns whether anything has been processed. For efficiency if nothing should be altered, return false,
    ///          and it will act as if bypassed. Return true otherwise when data has been processed normally.
    virtual auto process(const Float *input, Float *output, Int64 count) -> Bool = 0;

    AudioContext *m_context;
};

KSND_NS_END
