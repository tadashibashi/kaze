#pragma once
#include <kaze/audio/lib.h>
#include <kaze/audio/engine/AudioEffect.h>

KAUDIO_NS_BEGIN

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

KAUDIO_NS_END
