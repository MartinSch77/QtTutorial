// SPDX-License-Identifier: MIT
#include "EnergyMonitor.h"

#include <algorithm>

namespace qttutorial::homeautomation::remote {

double EnergyMonitor::estimateWatts(const Snapshot& snapshot)
{
    double watts = kBaseLoadWatts;

    for (const RoomState& room : snapshot.rooms) {
        if (room.lightOn) {
            watts += kWattsPerLightAtFullBrightness * (std::clamp(room.brightness, 0, 100) / 100.0);
        }
    }

    // thermostatMode matches ThermostatController::Mode: Off=0, Heat=1, Cool=2.
    if (snapshot.thermostatMode != 0) {
        watts += kHeatCoolWatts;
    }

    return watts;
}

EnergyHistory::EnergyHistory(std::size_t capacity)
    : m_capacity(capacity)
{
}

void EnergyHistory::addSample(double watts, qint64 timestampMs)
{
    m_samples.push_back(EnergySample{.watts = watts, .timestampMs = timestampMs});
    if (m_samples.size() > m_capacity) {
        m_samples.erase(m_samples.begin());
    }
}

double EnergyHistory::averageWatts() const
{
    if (m_samples.empty()) {
        return 0.0;
    }
    double total = 0.0;
    for (const EnergySample& sample : m_samples) {
        total += sample.watts;
    }
    return total / static_cast<double>(m_samples.size());
}

double EnergyHistory::peakWatts() const
{
    double peak = 0.0;
    for (const EnergySample& sample : m_samples) {
        peak = std::max(peak, sample.watts);
    }
    return peak;
}

} // namespace qttutorial::homeautomation::remote
