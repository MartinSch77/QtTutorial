// SPDX-License-Identifier: MIT
#include "GroundTrackMath.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::space {

namespace {
constexpr double kDegToRad = std::numbers::pi / 180.0;
constexpr double kRadToDeg = 180.0 / std::numbers::pi;

double wrapTo180(double degrees)
{
    double wrapped = std::fmod(degrees + 180.0, 360.0);
    if (wrapped < 0.0) {
        wrapped += 360.0;
    }
    return wrapped - 180.0;
}
}

GroundTrackPosition computeGroundTrack(double phaseDeg, double inclinationDeg, double earthRotationDeg)
{
    const double incRad = inclinationDeg * kDegToRad;
    const double vRad = phaseDeg * kDegToRad;

    const double latRad = std::asin(std::sin(incRad) * std::sin(vRad));
    const double lonFromNodeDeg = std::atan2(std::cos(incRad) * std::sin(vRad), std::cos(vRad)) * kRadToDeg;

    GroundTrackPosition position;
    position.latitudeDeg = latRad * kRadToDeg;
    position.longitudeDeg = wrapTo180(lonFromNodeDeg - earthRotationDeg);
    return position;
}

double angularDistanceDeg(double lat1Deg, double lon1Deg, double lat2Deg, double lon2Deg)
{
    const double lat1 = lat1Deg * kDegToRad;
    const double lat2 = lat2Deg * kDegToRad;
    const double dLon = (lon1Deg - lon2Deg) * kDegToRad;

    double cosAngle = std::sin(lat1) * std::sin(lat2) + std::cos(lat1) * std::cos(lat2) * std::cos(dLon);
    cosAngle = std::clamp(cosAngle, -1.0, 1.0);
    return std::acos(cosAngle) * kRadToDeg;
}

} // namespace qttutorial::space
