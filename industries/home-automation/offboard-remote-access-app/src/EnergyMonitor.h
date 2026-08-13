// SPDX-License-Identifier: MIT
#pragma once

#include "Protocol.h"

#include <cstddef>
#include <vector>

namespace qttutorial::homeautomation::remote {

// One point on the energy-usage trend chart: an estimated instantaneous draw
// and when it was observed.
struct EnergySample {
    double watts = 0.0;
    qint64 timestampMs = 0;
};

// Estimates the home's instantaneous power draw from a Snapshot. Pure
// function of the snapshot's own fields - no Qt GUI/network dependency, so
// it is unit testable independent of HomeSimulator or any live connection.
// The model is deliberately simple (a household base load, a per-light
// contribution that scales with brightness, and a flat contribution while
// the thermostat is actively heating/cooling) but it makes the same kind of
// state changes a real remote-access app's users care about ("I turned the
// lights off and it's cheaper now") visible and directionally correct.
class EnergyMonitor {
public:
    static constexpr double kBaseLoadWatts = 120.0;
    static constexpr double kWattsPerLightAtFullBrightness = 9.0;
    static constexpr double kHeatCoolWatts = 1500.0;

    [[nodiscard]] static double estimateWatts(const Snapshot& snapshot);
};

// A fixed-capacity rolling history of energy samples (oldest samples are
// dropped once `capacity` is reached), so a remote app can plot a trend
// chart without an unbounded memory footprint - the same reasoning
// ActivityLogStore's recentEntries(limit) uses for its query cap, just kept
// in memory instead of SQLite since this is a derived/estimated value, not
// durable history.
class EnergyHistory {
public:
    explicit EnergyHistory(std::size_t capacity = 60);

    void addSample(double watts, qint64 timestampMs);

    [[nodiscard]] const std::vector<EnergySample>& samples() const { return m_samples; }
    [[nodiscard]] double averageWatts() const;
    [[nodiscard]] double peakWatts() const;

private:
    std::size_t m_capacity;
    std::vector<EnergySample> m_samples;
};

} // namespace qttutorial::homeautomation::remote
