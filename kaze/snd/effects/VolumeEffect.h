#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioEffect.h>

KSND_NS_BEGIN

class VolumeEffect final : public AudioEffect {
public:
    VolumeEffect()
        : m_volume(1.f)
    {
    }

    VolumeEffect(VolumeEffect &&other) noexcept;

    auto init_(const Float volume = 1.f) -> Bool
    {
        m_volume = volume;

        return true;
    }

    explicit VolumeEffect(const Float volume) : m_volume(volume) { }

    auto process(const Float *input, Float *output, Int64 count) -> Bool override;

    [[nodiscard]]
    auto volume() const -> Float { return m_volume; }
    auto volume(Float value) -> void;

private:
    auto receiveParam(Int index, const Param &value) -> void override;

    enum Enum {
        Volume,
    };

    float m_volume;
};

KSND_NS_END
