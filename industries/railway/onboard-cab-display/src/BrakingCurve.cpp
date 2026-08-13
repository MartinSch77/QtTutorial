// SPDX-License-Identifier: MIT
#include "BrakingCurve.h"

#include <cmath>

namespace qttutorial::cab_display {

namespace {
constexpr double kKmhToMs = 1000.0 / 3600.0;
constexpr double kMsToKmh = 3600.0 / 1000.0;
}

double maxSafeSpeedKmh(double distanceToRestrictionM, double targetSpeedKmh, double decelerationMs2)
{
    if (distanceToRestrictionM <= 0.0) {
        return targetSpeedKmh;
    }
    const double targetMs = targetSpeedKmh * kKmhToMs;
    const double maxMs = std::sqrt(targetMs * targetMs + 2.0 * decelerationMs2 * distanceToRestrictionM);
    return maxMs * kMsToKmh;
}

bool nextBrakeWarningState(bool currentlyActive, double speedKmh, double permittedSpeedKmh, double onMarginKmh,
                            double offMarginKmh)
{
    if (currentlyActive) {
        return speedKmh > permittedSpeedKmh + offMarginKmh;
    }
    return speedKmh > permittedSpeedKmh + onMarginKmh;
}

} // namespace qttutorial::cab_display
