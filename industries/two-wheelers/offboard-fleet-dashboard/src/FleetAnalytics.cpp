// SPDX-License-Identifier: MIT
#include "FleetAnalytics.h"

namespace qttutorial::two_wheelers::fleet {

double utilizationPercent(const std::vector<VehicleSample>& samples)
{
    if (samples.empty()) {
        return 0.0;
    }
    int ridingCount = 0;
    for (const VehicleSample& sample : samples) {
        if (sample.status == RiderStatus::Riding) {
            ++ridingCount;
        }
    }
    return 100.0 * static_cast<double>(ridingCount) / static_cast<double>(samples.size());
}

FleetUtilizationHistory::FleetUtilizationHistory(std::size_t capacity)
    : m_capacity(capacity > 0 ? capacity : 1)
{
}

void FleetUtilizationHistory::addSample(double utilizationPercentValue)
{
    m_samples.push_back(utilizationPercentValue);
    while (m_samples.size() > m_capacity) {
        m_samples.pop_front();
    }
}

std::vector<double> FleetUtilizationHistory::values() const
{
    return std::vector<double>(m_samples.begin(), m_samples.end());
}

} // namespace qttutorial::two_wheelers::fleet
