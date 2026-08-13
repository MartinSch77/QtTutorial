// SPDX-License-Identifier: MIT
#include "StatsCompute.h"

#include <algorithm>
#include <cmath>
#include <ranges>

namespace qttutorial::concurrency_async {

void ChannelStatsAccumulator::add(double value)
{
    ++m_count;
    m_min = std::min(m_min, value);
    m_max = std::max(m_max, value);
    m_sum += value;
    m_sumSq += value * value;
}

ChannelStats ChannelStatsAccumulator::finish(int channel) const
{
    if (m_count == 0) {
        return ChannelStats{.channel = channel, .min = 0.0, .max = 0.0, .mean = 0.0, .stddev = 0.0, .sampleCount = 0};
    }
    const double mean = m_sum / static_cast<double>(m_count);
    const double variance = m_sumSq / static_cast<double>(m_count) - mean * mean;
    const double stddev = std::sqrt(std::max(variance, 0.0));
    return ChannelStats{
        .channel = channel,
        .min = m_min,
        .max = m_max,
        .mean = mean,
        .stddev = stddev,
        .sampleCount = m_count,
    };
}

ChannelStats computeChannelStats(const std::vector<SensorSample>& samples, int channel)
{
    ChannelStatsAccumulator accumulator;
    for (const double value : samples
                                   | std::views::filter([channel](const SensorSample& s) { return s.channel == channel; })
                                   | std::views::transform(&SensorSample::value)) {
        accumulator.add(value);
    }
    return accumulator.finish(channel);
}

std::vector<ChannelStats> computeAllChannelStats(const std::vector<SensorSample>& samples, int channelCount)
{
    std::vector<ChannelStats> results;
    results.reserve(static_cast<std::size_t>(std::max(channelCount, 0)));
    for (int channel = 0; channel < channelCount; ++channel) {
        results.push_back(computeChannelStats(samples, channel));
    }
    return results;
}

} // namespace qttutorial::concurrency_async
