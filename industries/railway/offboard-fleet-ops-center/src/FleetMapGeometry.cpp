// SPDX-License-Identifier: MIT
#include "FleetMapGeometry.h"

#include <cmath>
#include <numbers>

namespace qttutorial::fleet_ops {

MapPoint positionOnLoop(double positionKm, double loopLengthKm, double radius)
{
    if (loopLengthKm <= 0.0) {
        return MapPoint{0.0, -radius};
    }
    double fraction = positionKm / loopLengthKm;
    fraction = fraction - std::floor(fraction);
    const double angle = fraction * 2.0 * std::numbers::pi - (std::numbers::pi / 2.0);
    return MapPoint{radius * std::cos(angle), radius * std::sin(angle)};
}

} // namespace qttutorial::fleet_ops
