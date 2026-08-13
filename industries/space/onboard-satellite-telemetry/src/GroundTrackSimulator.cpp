// SPDX-License-Identifier: MIT
#include "GroundTrackSimulator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::space {

namespace {
constexpr double kDegToRad = std::numbers::pi / 180.0;
constexpr double kRadToDeg = 180.0 / std::numbers::pi;
// A real LEO ground track drifts westward roughly one Earth-rotation per
// orbit-ish; this constant just needs to be slow relative to the orbital
// period so a handful of orbits visibly precess rather than overlap exactly.
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

GroundTrackSimulator::GroundTrackSimulator(double inclinationDeg)
    : m_inclinationDeg(inclinationDeg)
{
}

void GroundTrackSimulator::step(double dtSeconds, double trueAnomalyDeg)
{
    m_earthRotationDeg = std::fmod(m_earthRotationDeg + kEarthRotationDegPerSecond * dtSeconds, 360.0);

    const double incRad = m_inclinationDeg * kDegToRad;
    const double vRad = trueAnomalyDeg * kDegToRad;

    // Sub-satellite latitude and the angular distance along the equator from
    // the ascending node, from the standard spherical-triangle relations for
    // an inclined circular orbit.
    const double latRad = std::asin(std::sin(incRad) * std::sin(vRad));
    const double lonFromNodeDeg = std::atan2(std::cos(incRad) * std::sin(vRad), std::cos(vRad)) * kRadToDeg;

    m_state.latitudeDeg = latRad * kRadToDeg;
    m_state.longitudeDeg = wrapTo180(lonFromNodeDeg - m_earthRotationDeg);
}

bool isWithinStationView(const GroundTrackState& track, double stationLatitudeDeg, double stationLongitudeDeg,
                          double maxAngularRadiusDeg)
{
    // Spherical law of cosines for the great-circle angular separation.
    const double lat1 = track.latitudeDeg * kDegToRad;
    const double lat2 = stationLatitudeDeg * kDegToRad;
    const double dLon = (track.longitudeDeg - stationLongitudeDeg) * kDegToRad;

    double cosAngle = std::sin(lat1) * std::sin(lat2) + std::cos(lat1) * std::cos(lat2) * std::cos(dLon);
    cosAngle = std::clamp(cosAngle, -1.0, 1.0);
    const double angularDistanceDeg = std::acos(cosAngle) * kRadToDeg;
    return angularDistanceDeg <= maxAngularRadiusDeg;
}

} // namespace qttutorial::space
