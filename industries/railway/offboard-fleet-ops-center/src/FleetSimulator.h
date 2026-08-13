// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::fleet_ops {

struct FleetConfig {
    int trainCount = 5;
    double loopLengthKm = 60.0;
    double baseSpeedKmh = 90.0;
    double stationSpacingKm = 6.0;
    double speedVariationKmh = 25.0;
    double delayAmplitudeMinutes = 6.0;
    double delayPeriodSeconds = 900.0;
};

struct TrainState {
    QString trainId;
    double positionKm = 0.0;
    double speedKmh = 0.0;
    double delayMinutes = 0.0;
    int nextStopIndex = 0;
    double distanceToNextStopKm = 0.0;
};

// Positions every train on a shared loop by integrating a constant base
// speed over elapsed time (so position and speed stay geometrically
// consistent), with a station-proximity speed dip and a slowly drifting,
// per-train-phased schedule-adherence signal standing in for real dwell-time
// variability — structured, not unstructured noise.
[[nodiscard]] TrainState trainStateAt(const FleetConfig& config, int trainIndex, double tSeconds);

[[nodiscard]] std::vector<TrainState> fleetStateAt(const FleetConfig& config, double tSeconds);

} // namespace qttutorial::fleet_ops
