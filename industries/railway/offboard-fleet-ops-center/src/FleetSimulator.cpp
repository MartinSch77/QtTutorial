// SPDX-License-Identifier: MIT
#include "FleetSimulator.h"

#include <cmath>
#include <numbers>

namespace qttutorial::fleet_ops {

namespace {
double wrap(double value, double modulus)
{
    double result = std::fmod(value, modulus);
    if (result < 0.0) {
        result += modulus;
    }
    return result;
}
}

TrainState trainStateAt(const FleetConfig& config, int trainIndex, double tSeconds)
{
    const double phaseOffsetKm = config.loopLengthKm * static_cast<double>(trainIndex)
        / static_cast<double>(std::max(config.trainCount, 1));
    const double positionKm = wrap(phaseOffsetKm + config.baseSpeedKmh * (tSeconds / 3600.0), config.loopLengthKm);

    const double stationPhase = 2.0 * std::numbers::pi * (positionKm / config.stationSpacingKm);
    const double speedKmh = std::max(5.0, config.baseSpeedKmh - config.speedVariationKmh
                                               * (0.5 + 0.5 * std::sin(stationPhase - std::numbers::pi / 2.0)));

    const double delayPhase = 2.0 * std::numbers::pi * (tSeconds / config.delayPeriodSeconds)
        + static_cast<double>(trainIndex) * (std::numbers::pi / 3.0);
    const double delayMinutes = config.delayAmplitudeMinutes * std::sin(delayPhase);

    const int nextStopIndex = static_cast<int>(std::floor(positionKm / config.stationSpacingKm)) + 1;
    const double distanceToNextStopKm = nextStopIndex * config.stationSpacingKm - positionKm;

    TrainState state;
    state.trainId = QStringLiteral("T-%1").arg(trainIndex + 1, 2, 10, QLatin1Char('0'));
    state.positionKm = positionKm;
    state.speedKmh = speedKmh;
    state.delayMinutes = delayMinutes;
    state.nextStopIndex = nextStopIndex;
    state.distanceToNextStopKm = distanceToNextStopKm;
    return state;
}

std::vector<TrainState> fleetStateAt(const FleetConfig& config, double tSeconds)
{
    std::vector<TrainState> states;
    states.reserve(static_cast<std::size_t>(config.trainCount));
    for (int i = 0; i < config.trainCount; ++i) {
        states.push_back(trainStateAt(config, i, tSeconds));
    }
    return states;
}

} // namespace qttutorial::fleet_ops
