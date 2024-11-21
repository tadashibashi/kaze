#pragma once
#include <kaze/snd/lib.h>
#include <kaze/snd/AudioEffect.h>

KSND_NS_BEGIN

class PanEffect : public AudioEffect {
public:
    PanEffect() : m_left(1.f), m_right(1.f) { }
    PanEffect(Float left, Float right) : m_left(left), m_right(right) { }
    PanEffect(PanEffect &&other) noexcept;

    auto process(const Float *input, Float *output, Int64 count) -> Bool override;

    auto init_(Float left = 1.f, Float right = 1.f) -> Bool
    {
        m_left = left;
        m_right = right;
        return True;
    }

    // ----- getters / setters -----

    auto left(Float value) -> void;
    auto right(Float value) -> void;

    [[nodiscard]]
    auto left() const { return m_left; }
    [[nodiscard]]
    auto right() const { return m_right; }

private:
    enum Enum {
        Left,
        Right
    };

    auto receiveParam(Int index, const Param &value) -> void override;

    float m_left, m_right;
};

KSND_NS_END
