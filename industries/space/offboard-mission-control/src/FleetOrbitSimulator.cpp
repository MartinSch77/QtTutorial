// SPDX-License-Identifier: MIT
#include "FleetOrbitSimulator.h"

#include "GroundTrackMath.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::space {

namespace {
constexpr double kMaxPanelOutputWatts = 320.0;
constexpr double kBusLoadWatts = 150.0;
constexpr double kBatteryCapacityWattHours = 120.0;
// Slow relative to the (sped-up) orbital periods below, so the fleet's
// ground tracks visibly drift westward orbit over orbit rather than
// retracing the same line, matching real LEO ground-track plots.
constexpr double kEarthRotationDegPerSecond = 360.0 / (24.0 * 60.0 * 60.0);

double solarInputFraction(double phaseDeg, double eclipseFraction)
{
    const double eclipseHalfWidthDeg = eclipseFraction * 180.0;
    const double distanceFromEclipseCenter = std::abs(std::fmod(phaseDeg - 180.0 + 540.0, 360.0) - 180.0);
    if (distanceFromEclipseCenter < eclipseHalfWidthDeg) {
        return 0.0;
    }
    const double fadeZoneDeg = 5.0;
    return std::clamp((distanceFromEclipseCenter - eclipseHalfWidthDeg) / fadeZoneDeg, 0.0, 1.0);
}
}

FleetOrbitSimulator::FleetOrbitSimulator()
{
    const struct {
        const char* name;
        double phaseDeg;
        double periodMinutes;
        double eclipseFraction;
        double startBattery;
        double inclinationDeg;
    } fleet[] = {
        {"AURORA-1", 0.0, 95.0, 0.35, 88.0, 53.0},
        {"AURORA-2", 60.0, 98.0, 0.34, 72.0, 53.0},
        {"AURORA-3", 150.0, 92.0, 0.36, 45.0, 82.0},
        {"AURORA-4", 220.0, 100.0, 0.33, 25.0, 45.0},
        {"AURORA-5", 300.0, 96.0, 0.35, 91.0, 97.4}, // sun-synchronous-like
    };

    for (const auto& entry : fleet) {
        SatelliteState state;
        state.name = QString::fromLatin1(entry.name);
        state.phaseDeg = entry.phaseDeg;
        state.batteryPercent = entry.startBattery;
        state.inclinationDeg = entry.inclinationDeg;
        state.orbitalPeriodMinutes = entry.periodMinutes;
        const GroundTrackPosition track = computeGroundTrack(state.phaseDeg, state.inclinationDeg, 0.0);
        state.latitudeDeg = track.latitudeDeg;
        state.longitudeDeg = track.longitudeDeg;
        m_satellites.push_back(state);
        m_orbitalPeriodMinutes.push_back(entry.periodMinutes);
        m_eclipseFraction.push_back(entry.eclipseFraction);
    }
}

void FleetOrbitSimulator::step(double dtSeconds)
{
    m_earthRotationDeg = std::fmod(m_earthRotationDeg + kEarthRotationDegPerSecond * dtSeconds, 360.0);

    for (std::size_t i = 0; i < m_satellites.size(); ++i) {
        SatelliteState& satellite = m_satellites[i];
        const double periodSeconds = m_orbitalPeriodMinutes[i] * 60.0;
        satellite.phaseDeg = std::fmod(satellite.phaseDeg + 360.0 / periodSeconds * dtSeconds, 360.0);

        const double solarFraction = solarInputFraction(satellite.phaseDeg, m_eclipseFraction[i]);
        satellite.inEclipse = solarFraction <= 0.0;

        const double panelWatts = solarFraction * kMaxPanelOutputWatts;
        const double netWattHours = (panelWatts - kBusLoadWatts) * (dtSeconds / 3600.0);
        const double socDelta = netWattHours / kBatteryCapacityWattHours * 100.0;
        satellite.batteryPercent = std::clamp(satellite.batteryPercent + socDelta, 0.0, 100.0);

        satellite.health = classifyHealth(satellite.batteryPercent, satellite.inEclipse);

        const GroundTrackPosition track = computeGroundTrack(satellite.phaseDeg, satellite.inclinationDeg,
                                                               m_earthRotationDeg);
        satellite.latitudeDeg = track.latitudeDeg;
        satellite.longitudeDeg = track.longitudeDeg;
    }
}

} // namespace qttutorial::space
