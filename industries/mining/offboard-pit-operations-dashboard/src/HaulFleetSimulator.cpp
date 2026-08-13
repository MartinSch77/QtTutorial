// SPDX-License-Identifier: MIT
#include "HaulFleetSimulator.h"

#include <cmath>

namespace qttutorial::mining::pit {

double HaulFleetSimulator::phaseOffsetFor(int truckIndex)
{
    return static_cast<double>(truckIndex) * 41.0;
}

int HaulFleetSimulator::stateIndexAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t < kLoadingSeconds) {
        return 0; // Loading
    }
    if (t < kLoadingSeconds + kHaulingSeconds) {
        return 1; // Hauling
    }
    if (t < kLoadingSeconds + kHaulingSeconds + kDumpingSeconds) {
        return 2; // Dumping
    }
    return 3; // Returning
}

QString HaulFleetSimulator::stateLabelFor(int stateIndex)
{
    switch (stateIndex) {
    case 0:
        return QStringLiteral("Loading");
    case 1:
        return QStringLiteral("Hauling");
    case 2:
        return QStringLiteral("Dumping");
    default:
        return QStringLiteral("Returning");
    }
}

double HaulFleetSimulator::payloadAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t < kLoadingSeconds) {
        return kRatedCapacityTonnes * (t / kLoadingSeconds);
    }
    if (t < kLoadingSeconds + kHaulingSeconds) {
        return kRatedCapacityTonnes;
    }
    if (t < kLoadingSeconds + kHaulingSeconds + kDumpingSeconds) {
        const double td = t - (kLoadingSeconds + kHaulingSeconds);
        return kRatedCapacityTonnes * (1.0 - td / kDumpingSeconds);
    }
    return 0.0;
}

QString HaulFleetSimulator::locationFor(int truckIndex, int stateIndex)
{
    const bool alternate = (truckIndex % 2) == 0;
    switch (stateIndex) {
    case 0:
        return alternate ? QStringLiteral("Loading Shovel 1 - Bench 4") : QStringLiteral("Loading Shovel 2 - Bench 6");
    case 1:
        return alternate ? QStringLiteral("Haul Road North") : QStringLiteral("Haul Road South");
    case 2:
        return alternate ? QStringLiteral("Crusher Tip") : QStringLiteral("Waste Dump");
    default:
        return alternate ? QStringLiteral("Haul Road North (empty)") : QStringLiteral("Haul Road South (empty)");
    }
}

TruckSample HaulFleetSimulator::sampleAt(int truckIndex, double elapsedSeconds)
{
    const double effective = elapsedSeconds + phaseOffsetFor(truckIndex);
    const double phase = std::fmod(effective, kCyclePeriodSeconds);
    const int stateIndex = stateIndexAt(phase);

    TruckSample sample;
    sample.id = QStringLiteral("MT-%1").arg(truckIndex + 1, 3, 10, QLatin1Char('0'));
    sample.location = locationFor(truckIndex, stateIndex);
    sample.stateIndex = stateIndex;
    sample.stateLabel = stateLabelFor(stateIndex);
    sample.payloadTonnes = payloadAt(phase);
    sample.cumulativeTonnesHauled = std::floor(effective / kCyclePeriodSeconds) * kRatedCapacityTonnes;
    return sample;
}

} // namespace qttutorial::mining::pit
