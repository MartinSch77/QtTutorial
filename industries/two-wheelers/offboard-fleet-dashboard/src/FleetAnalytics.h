// SPDX-License-Identifier: MIT
#pragma once

#include "FleetVehicleSimulator.h"

#include <deque>
#include <vector>

namespace qttutorial::two_wheelers::fleet {

// Pure aggregation over a snapshot of the fleet: the percentage of vehicles
// currently Riding. Kept as a free function (no QObject/QTimer dependency) so it
// is trivially unit-testable against a hand-built vector of VehicleSample.
[[nodiscard]] double utilizationPercent(const std::vector<VehicleSample>& samples);

// A bounded rolling history of fleet-wide utilization samples, used to feed the
// "fleet-wide utilization trend" chart in the back-office dashboard. Independent
// of any UI widget - it just keeps the last N samples (oldest first) so the
// dashboard can plot a trend without an unbounded, ever-growing buffer.
class FleetUtilizationHistory {
public:
    explicit FleetUtilizationHistory(std::size_t capacity = 120);

    void addSample(double utilizationPercent);

    [[nodiscard]] std::vector<double> values() const;
    [[nodiscard]] std::size_t size() const { return m_samples.size(); }
    [[nodiscard]] std::size_t capacity() const { return m_capacity; }

private:
    std::size_t m_capacity;
    std::deque<double> m_samples;
};

} // namespace qttutorial::two_wheelers::fleet
