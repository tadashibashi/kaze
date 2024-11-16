#pragma once
#include <kaze/audio/lib.h>
#include <kaze/audio/engine/AudioEffect.h>

KAUDIO_NS_BEGIN

class DelayEffect final : public AudioEffect {
    public:
        DelayEffect();
        ~DelayEffect() override = default;
        DelayEffect(DelayEffect &&other) noexcept;

        /// Initialize the DelayEffect
        /// @param delayTime number of samples to delay
        /// @param wet       percentage of the effect signal to output, dry signal is calculated as `1.f - wet`.
        /// @param feedback  percentage of signal to capture
        bool init_(Uint64 delayTime, Float wet, Float feedback);

        bool process(const Float *input, Float *output, Int64 count) override;

        /// Set the delay time in sample frames, (use engine spec to find sample rate)
        void delayTime(Uint64 samples);

        /// Get the current delay time in sample frames
        [[nodiscard]]
        auto delayTime() const { return m_delayTime; }

        /// Set the input feedback percentage, where 0 = 0% through 1.f = 100%
        void feedback(Float value);

        /// Get the input feedback percentage, where 0 = 0% through 1.f = 100%
        [[nodiscard]]
        Float feedback() const { return m_feedback; }

        /// Set the wet and dry signal percentages at once, where the value of the wet signal is equal to `value`,
        /// and dry is `1.f - value`.
        /// For example, 0 results in 0% wet signal (no effect applied with 100% dry
        /// signal), and 1.f results in 100% of the wet signal and 0% of the dry.
        void wetDry(Float value);

        /// Get the wet and dry signal percentages at once, where the value of the wet signal is equal to `value`,
        /// and dry is `1.f - value`.
        /// For example, 0 results in 0% wet signal (no effect applied with 100% dry
        /// signal), and 1.f results in 100% of the wet signal and 0% of the dry.
        [[nodiscard]]
        Float wetDry() const { return m_wet; }

    protected:
        enum Enum
        {
            DelayTime,
            Feedback,
            Wet,
        };


        auto receiveParam(Int index, const Param &value) -> void override;

    private:
        AlignedList<Float, 16> m_buffer;
        Uint64 m_delayTime; // delay time in sample frames
        Float m_feedback;
        Float m_wet;

        Uint64 m_delayHead;
    };

KAUDIO_NS_END
