// SPDX-License-Identifier: MIT
#pragma once

#include "SensorData.h"

#include <limits>
#include <vector>

namespace qttutorial::concurrency_async {

// Incremental single-pass accumulator: used both by the QtConcurrent map function
// (one instance per channel, per worker thread, no shared state) and by the
// std::jthread worker (fed sample-by-sample so progress/cancellation stay fine-grained).
class ChannelStatsAccumulator {
public:
    void add(double value);
    [[nodiscard]] ChannelStats finish(int channel) const;

private:
    std::size_t m_count = 0;
    double m_min = std::numeric_limits<double>::infinity();
    double m_max = -std::numeric_limits<double>::infinity();
    double m_sum = 0.0;
    double m_sumSq = 0.0;
};

[[nodiscard]] ChannelStats computeChannelStats(const std::vector<SensorSample>& samples, int channel);

[[nodiscard]] std::vector<ChannelStats> computeAllChannelStats(const std::vector<SensorSample>& samples,
                                                                 int channelCount);

} // namespace qttutorial::concurrency_async
