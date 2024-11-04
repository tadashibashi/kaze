#include "FramerateCounter.h"
#include <kaze/core/math/mathf.h>
#include <kaze/core/memory.h>

KAZE_NS_BEGIN

FramerateCounter::FramerateCounter(const FramerateCounterInit &config) :
    m_samples(config.samples, 0), m_total(0), m_framesCounted(0),
    m_head(), m_lastFrameTime()
{
    KAZE_ASSERT(config.samples > 0);

    m_head = m_samples.data();
}

auto FramerateCounter::frame() -> void
{
    const auto frameTime = Clock::now();

    const std::chrono::duration<double> elapsed = frameTime - m_lastFrameTime;
    m_lastFrameTime = frameTime;

    const auto elapsedSeconds = elapsed.count();

    // calculate windowed total
    m_total -= *m_head;
    m_total += elapsedSeconds;
    *m_head = elapsedSeconds;

    // shift window
    const auto end = m_samples.data() + m_samples.size();
    if (m_head + 1 >= end)
        m_head = m_samples.data();
    else
        ++m_head;

    ++m_framesCounted;
}

auto FramerateCounter::reset(std::optional<FramerateCounterInit> config) -> void
{
    if (config)
    {
        KAZE_ASSERT(config-> samples > 0);
        m_samples.assign(config->samples, 0);
    }
    else
    {
        memory::set(m_samples.data(), 0, m_samples.size() * sizeof(*m_samples.data()));
    }

    m_head = m_samples.data();
    m_total = 0;
    m_framesCounted = 0;
    m_lastFrameTime = Clock::now();
}

auto FramerateCounter::getAverageSpf() const -> Double
{
    auto sampleSize = mathf::min<Int64>(m_framesCounted, m_samples.size());
    if (sampleSize == 0)
        return 0;
    return m_total / static_cast<Double>(sampleSize);
}

auto FramerateCounter::getAverageFps() const -> Double
{
    const auto spf = getAverageSpf();
    return (spf == 0) ? 0 : 1.0 / spf;
}

auto FramerateCounter::getDeltaTime() const -> Double
{
    return *m_head;
}

KAZE_NS_END


