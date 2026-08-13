// SPDX-License-Identifier: MIT
#include "GroundStationTracker.h"

#include "GroundTrackMath.h"

#include <cmath>

namespace qttutorial::space {

namespace {
// The same Earth-rotation rate FleetOrbitSimulator advances the fleet's
// ground tracks by, so forward-propagated predictions stay consistent with
// the live simulation.
constexpr double kEarthRotationDegPerSecond = 360.0 / (24.0 * 60.0 * 60.0);

double wrapTo180(double degrees)
{
    double wrapped = std::fmod(degrees + 180.0, 360.0);
    if (wrapped < 0.0) {
        wrapped += 360.0;
    }
    return wrapped - 180.0;
}
}

GroundStationTracker::GroundStationTracker()
{
    // A small, fictitious constellation of ground stations - not real
    // facilities or coordinates - just enough geographic spread to
    // demonstrate a plausible "next contact" schedule across a fleet in
    // varied inclinations.
    m_stations.push_back({QStringLiteral("Northgate"), 51.0, -1.0, 12.0});
    m_stations.push_back({QStringLiteral("Meridian"), 5.0, 45.0, 12.0});
    m_stations.push_back({QStringLiteral("Southpoint"), -34.0, 151.0, 12.0});
    m_stations.push_back({QStringLiteral("Polaris Ridge"), 64.0, -21.0, 14.0});
}

std::optional<ContactWindow> GroundStationTracker::nextContact(const SatelliteState& satellite,
                                                                 double orbitalPeriodMinutes, double stepMinutes,
                                                                 double maxLookaheadMinutes) const
{
    for (const GroundStation& station : m_stations) {
        const double distance =
            angularDistanceDeg(satellite.latitudeDeg, satellite.longitudeDeg, station.latitudeDeg, station.longitudeDeg);
        if (distance <= station.visibilityRadiusDeg) {
            ContactWindow window;
            window.satelliteName = satellite.name;
            window.stationName = station.name;
            window.inContactNow = true;
            window.minutesUntilNextContact = 0.0;
            return window;
        }
    }

    // Baseline "lon from node" for the current phase, used only to compute a
    // *relative* longitude drift as phase and Earth rotation both advance -
    // this avoids needing to know the simulator's absolute accumulated Earth
    // -rotation angle here, at the cost of a small amount of drift error near
    // the +/-180 degree seam, acceptable for an illustrative schedule.
    const double baselineLonFromNode = computeGroundTrack(satellite.phaseDeg, satellite.inclinationDeg, 0.0).longitudeDeg;
    const double periodSeconds = orbitalPeriodMinutes * 60.0;
    const double degreesPerSecond = 360.0 / periodSeconds;

    double bestMinutes = -1.0;
    QString bestStation;

    // Step by integer sample count rather than accumulating a floating-point
    // loop counter, to avoid drift/rounding in the loop bound itself.
    const int sampleCount = static_cast<int>(maxLookaheadMinutes / stepMinutes);
    for (int sample = 1; sample <= sampleCount; ++sample) {
        const double elapsedMinutes = sample * stepMinutes;
        const double elapsedSeconds = elapsedMinutes * 60.0;
        const double futurePhaseDeg = std::fmod(satellite.phaseDeg + degreesPerSecond * elapsedSeconds, 360.0);
        const double futureLonFromNode = computeGroundTrack(futurePhaseDeg, satellite.inclinationDeg, 0.0).longitudeDeg;
        const double earthRotationDeltaDeg = kEarthRotationDegPerSecond * elapsedSeconds;

        const double futureLat = computeGroundTrack(futurePhaseDeg, satellite.inclinationDeg, 0.0).latitudeDeg;
        const double futureLon =
            wrapTo180(satellite.longitudeDeg + (futureLonFromNode - baselineLonFromNode) - earthRotationDeltaDeg);

        for (const GroundStation& station : m_stations) {
            const double distance = angularDistanceDeg(futureLat, futureLon, station.latitudeDeg, station.longitudeDeg);
            if (distance <= station.visibilityRadiusDeg) {
                bestMinutes = elapsedMinutes;
                bestStation = station.name;
                break;
            }
        }
        if (bestMinutes >= 0.0) {
            break;
        }
    }

    if (bestMinutes < 0.0) {
        return std::nullopt;
    }

    ContactWindow window;
    window.satelliteName = satellite.name;
    window.stationName = bestStation;
    window.inContactNow = false;
    window.minutesUntilNextContact = bestMinutes;
    return window;
}

} // namespace qttutorial::space
