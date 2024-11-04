#pragma once
#include <kaze/core/lib.h>
#include <chrono>
#include <optional>

KAZE_NS_BEGIN

/// Initialization parameters for `FramerateCounter`
struct FramerateCounterInit {
    Int samples = 100;
};

/// Utility class that tracks average frame rate
class FramerateCounter {
public:
    FramerateCounter(const FramerateCounterInit &config = {});
    ~FramerateCounter() = default;

    /// Call this once (and only once) per frame somewhere in your app update loop
    auto frame() -> void;

    /// Reset the counter
    /// \note Must be called after the backend has been initialized
    /// \param[in]  config   Configuration object to reset the counter with [optional]
    auto reset(std::optional<FramerateCounterInit> config = {}) -> void;

    /// \returns average seconds per frame
    [[nodiscard]]
    auto getAverageSpf() const -> Double;

    /// \returns average frames per second
    [[nodiscard]]
    auto getAverageFps() const -> Double;

    /// \returns time passed in the last frame, in seconds
    [[nodiscard]]
    auto getDeltaTime() const -> Double;

private:
    using Clock = std::chrono::high_resolution_clock;

    List<Double> m_samples;
    Double *m_head;
    Double m_total;
    std::chrono::time_point<Clock> m_lastFrameTime;
    Uint64 m_framesCounted;
};

KAZE_NS_END
