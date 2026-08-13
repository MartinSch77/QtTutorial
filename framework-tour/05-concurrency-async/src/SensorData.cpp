// SPDX-License-Identifier: MIT
#include "SensorData.h"

#include <cmath>
#include <random>

namespace qttutorial::concurrency_async {

std::vector<SensorSample> generateSamples(unsigned seed, std::size_t sampleCount, int channelCount)
{
    std::vector<SensorSample> samples;
    samples.reserve(sampleCount);

    // std::mt19937 with a fixed seed is deterministic across runs/platforms for a
    // given libstdc++ version, which is what "reproducible for tests" requires here.
    std::mt19937 rng(seed);
    std::uniform_real_distribution<double> noise(-1.0, 1.0);

    for (std::size_t i = 0; i < sampleCount; ++i) {
        const int channel = channelCount > 0 ? static_cast<int>(i % static_cast<std::size_t>(channelCount)) : 0;
        const double phase = static_cast<double>(i) * 0.05 + channel * 0.7;
        const double value = std::sin(phase) * (5.0 + channel) + noise(rng);
        samples.push_back(SensorSample{
            .timestampMs = static_cast<qint64>(i) * 10,
            .channel = channel,
            .value = value,
        });
    }
    return samples;
}

} // namespace qttutorial::concurrency_async
