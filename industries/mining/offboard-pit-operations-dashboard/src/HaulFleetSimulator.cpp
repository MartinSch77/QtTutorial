// SPDX-License-Identifier: MIT
#include "HaulFleetSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::mining::pit {

namespace {
constexpr double kLoadingOverfillEndSeconds = 20.0;
} // namespace

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

    if (t < kLoadingOverfillEndSeconds) {
        // The shovel operator slightly overfills the tray before trimming it
        // back to rated capacity - the same believable brief overload spike
        // modelled by the onboard console's HaulCycleSimulator, so the fleet
        // view and the cab HMI agree on what "loading" looks like.
        return kPeakLoadingOverfillTonnes * (t / kLoadingOverfillEndSeconds);
    }
    if (t < kLoadingSeconds) {
        const double trim = (t - kLoadingOverfillEndSeconds) / (kLoadingSeconds - kLoadingOverfillEndSeconds);
        return kPeakLoadingOverfillTonnes - (kPeakLoadingOverfillTonnes - kRatedCapacityTonnes) * trim;
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

double HaulFleetSimulator::speedKphAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    constexpr double kLoadingCreepMaxKph = 8.0;
    constexpr double kHaulingCruiseKph = 45.0;
    constexpr double kHaulingAccelEndSeconds = 20.0;
    constexpr double kDescentSpeedKph = 25.0;
    constexpr double kDescentLeadSeconds = 20.0;
    constexpr double kDumpingBrakeSeconds = 5.0;
    constexpr double kReturningCruiseKph = 55.0;
    constexpr double kReturningAccelEndSeconds = 15.0;

    const double haulingEnd = kLoadingSeconds + kHaulingSeconds;
    const double dumpingEnd = haulingEnd + kDumpingSeconds;

    if (t < kLoadingSeconds) {
        const double half = kLoadingSeconds / 2.0;
        return t < half ? kLoadingCreepMaxKph * (t / half) : kLoadingCreepMaxKph * (1.0 - (t - half) / half);
    }
    if (t < haulingEnd) {
        const double th = t - kLoadingSeconds;
        const double descentStart = kHaulingSeconds - kDescentLeadSeconds;
        if (th < kHaulingAccelEndSeconds) {
            return kHaulingCruiseKph * (th / kHaulingAccelEndSeconds);
        }
        if (th < descentStart) {
            return kHaulingCruiseKph;
        }
        const double td = (th - descentStart) / kDescentLeadSeconds;
        return kHaulingCruiseKph - (kHaulingCruiseKph - kDescentSpeedKph) * td;
    }
    if (t < dumpingEnd) {
        const double td = t - haulingEnd;
        if (td < kDumpingBrakeSeconds) {
            return kDescentSpeedKph * (1.0 - td / kDumpingBrakeSeconds);
        }
        return 0.0;
    }

    const double tr = t - dumpingEnd;
    if (tr < kReturningAccelEndSeconds) {
        return kReturningCruiseKph * (tr / kReturningAccelEndSeconds);
    }
    return kReturningCruiseKph;
}

double HaulFleetSimulator::fuelLtrPerHourAt(double payloadTonnes, double speedKph)
{
    // Mirrors the onboard console's fuel model: a heavier payload and a
    // higher road speed both drive up fuel burn, rather than fuel varying
    // independently of what the truck is actually doing.
    constexpr double kIdleFuelLtrPerHour = 45.0;
    constexpr double kLoadFuelRangeLtrPerHour = 90.0;
    constexpr double kSpeedFuelRangeLtrPerHour = 40.0;
    constexpr double kReferenceMaxSpeedKph = 55.0;

    const double loadFraction = std::clamp(payloadTonnes / kRatedCapacityTonnes, 0.0, 1.05);
    const double speedFraction = std::clamp(speedKph / kReferenceMaxSpeedKph, 0.0, 1.2);
    return kIdleFuelLtrPerHour + kLoadFuelRangeLtrPerHour * loadFraction + kSpeedFuelRangeLtrPerHour * speedFraction;
}

bool HaulFleetSimulator::isOverloaded(double payloadTonnes)
{
    return payloadTonnes > kRatedCapacityTonnes;
}

void HaulFleetSimulator::positionFor(int truckIndex, int stateIndex, double phaseSeconds, double& outX, double& outY)
{
    // A small set of named "stations" laid out on a normalised 0..1 pit
    // plan: two loading shovels near the top, a haul road running down each
    // side of the pit, and a crusher/waste-dump pair at the bottom. Trucks
    // interpolate along the appropriate leg for their current phase and
    // elapsed time within it, so the pit-map view shows continuous motion
    // rather than trucks teleporting between fixed points.
    const bool alternate = (truckIndex % 2) == 0;
    const double laneX = alternate ? 0.28 : 0.72;
    const double shovelY = 0.12;
    const double dumpY = 0.88;

    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    switch (stateIndex) {
    case 0: { // Loading: stationary at the shovel.
        outX = laneX;
        outY = shovelY;
        return;
    }
    case 1: { // Hauling: travelling down the haul road from shovel to dump.
        const double th = t - kLoadingSeconds;
        const double progress = std::clamp(th / kHaulingSeconds, 0.0, 1.0);
        outX = laneX;
        outY = shovelY + (dumpY - shovelY) * progress;
        return;
    }
    case 2: { // Dumping: stationary at the dump/crusher.
        outX = laneX;
        outY = dumpY;
        return;
    }
    default: { // Returning: travelling back up the haul road, empty.
        const double td = t - (kLoadingSeconds + kHaulingSeconds + kDumpingSeconds);
        const double progress = std::clamp(td / kReturningSeconds, 0.0, 1.0);
        outX = laneX;
        outY = dumpY - (dumpY - shovelY) * progress;
        return;
    }
    }
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
    sample.speedKph = speedKphAt(phase);
    sample.fuelLtrPerHour = fuelLtrPerHourAt(sample.payloadTonnes, sample.speedKph);
    sample.overloaded = isOverloaded(sample.payloadTonnes);
    positionFor(truckIndex, stateIndex, phase, sample.positionX, sample.positionY);
    return sample;
}

} // namespace qttutorial::mining::pit
