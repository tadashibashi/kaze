#include "FramerateCounter.h"
#include <kaze/core/math/mathf.h>

KAZE_NS_BEGIN

FramerateCounter::FramerateCounter(const FramerateCounterInit &config) :
    m_samples(config.samples, {}), m_total(0)
{
    KAZE_ASSERT(config.samples > 1);

    m_head = m_samples.data();
}

auto FramerateCounter::frameBegin() -> void
{
    m_now = ClockType::now();
}

auto FramerateCounter::frameEnd() -> void
{
    const auto elapsed = ClockType::now() - m_now;
    const auto seconds = std::chrono::duration<Double>(elapsed).count();

    // calculate windowed total
    m_total -= *m_head;
    m_total += seconds;
    *m_head = seconds;

    // shift window
    const auto end = &(*m_samples.end());
    if (m_head + 1 >= end)
        m_head = m_samples.data();
    else
        m_head += 1;

    ++m_framesCounted;
}

auto FramerateCounter::getAverageSpf() const -> Double
{
    KAZE_ASSERT(!m_samples.empty());
    auto sampleSize = mathf::min<Int64>(m_framesCounted, m_samples.size());
    if (sampleSize == 0)
        return 0;
    return m_total / static_cast<Double>(sampleSize);
}

auto kaze::FramerateCounter::getAverageFps() const -> Double
{
    const auto spf = getAverageSpf();
    return (spf == 0) ? 0 : 1.0 / spf;
}

KAZE_NS_END


