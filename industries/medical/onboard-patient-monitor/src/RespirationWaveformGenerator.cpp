// SPDX-License-Identifier: MIT
#include "RespirationWaveformGenerator.h"

#include <cmath>
#include <numbers>

namespace qttutorial::medical {

namespace {
constexpr double kHalfPi = std::numbers::pi / 2.0;
// Real breathing is asymmetric: inspiration is comparatively quick,
// expiration is slower and more passive. Splitting the cycle unevenly (40%
// inspiration / 60% expiration) gives a recognisable respiration trace
// instead of a plain symmetric sine wave.
constexpr double kInspirationFraction = 0.4;
} // namespace

double RespirationWaveformGenerator::sample(double timeSeconds, double respirationRateBpm)
{
    const double cycleLength = 60.0 / respirationRateBpm;
    const double phase = std::fmod(timeSeconds, cycleLength) / cycleLength;

    if (phase < kInspirationFraction) {
        // Quick rise from 0 to 1 across the inspiratory portion of the cycle.
        return std::sin((phase / kInspirationFraction) * kHalfPi);
    }
    // Slower fall from 1 back to 0 across the expiratory portion.
    const double expiratoryPhase = (phase - kInspirationFraction) / (1.0 - kInspirationFraction);
    return std::cos(expiratoryPhase * kHalfPi);
}

} // namespace qttutorial::medical
