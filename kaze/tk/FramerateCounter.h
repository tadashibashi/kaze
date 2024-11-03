#pragma once
#include <kaze/core/lib.h>
#include <chrono>

KAZE_NS_BEGIN

struct FramerateCounterInit {
    Int samples = 100;
};

class FramerateCounter {
public:
    using ClockType = std::chrono::high_resolution_clock;

    FramerateCounter(const FramerateCounterInit &config = {});
    ~FramerateCounter() = default;

    auto frameBegin() -> void;
    auto frameEnd() -> void;

    /// \returns average seconds per frame
    [[nodiscard]]
    auto getAverageSpf() const -> Double;

    /// \returns average frames per second
    [[nodiscard]]
    auto getAverageFps() const -> Double;

   private:
    List<Double> m_samples;
    Double *m_head;
    Double m_total;
    std::chrono::time_point<ClockType> m_now;
    Uint64 m_framesCounted;
};

KAZE_NS_END
