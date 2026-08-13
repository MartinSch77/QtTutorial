// SPDX-License-Identifier: MIT
#pragma once

#include <QMetaType>
#include <QtGlobal>

#include <cstddef>
#include <vector>

namespace qttutorial::concurrency_async {

struct SensorSample {
    qint64 timestampMs = 0;
    int channel = 0;
    double value = 0.0;
};

struct ChannelStats {
    int channel = 0;
    double min = 0.0;
    double max = 0.0;
    double mean = 0.0;
    double stddev = 0.0;
    std::size_t sampleCount = 0;
};

// Deterministic for a given (seed, sampleCount, channelCount): same inputs always
// produce the same samples, which is what makes the pipelines below testable.
[[nodiscard]] std::vector<SensorSample> generateSamples(unsigned seed, std::size_t sampleCount,
                                                          int channelCount);

} // namespace qttutorial::concurrency_async

// Cross-thread signal emission (JThreadWorker's finished signal fires from a
// std::jthread) delivers arguments via a queued QMetaCallEvent, which requires
// the argument types to be known to Qt's meta-type system.
Q_DECLARE_METATYPE(qttutorial::concurrency_async::ChannelStats)
Q_DECLARE_METATYPE(std::vector<qttutorial::concurrency_async::ChannelStats>)
