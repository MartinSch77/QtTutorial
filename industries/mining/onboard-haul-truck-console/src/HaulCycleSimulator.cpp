// SPDX-License-Identifier: MIT
#include "HaulCycleSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::mining {

namespace {

constexpr double kLoadingOverfillEndSeconds = 20.0;
constexpr std::array<double, 6> kWheelPressureOffsetsKPa = {0.0, 0.0, 5.0, -5.0, 5.0, -5.0};

} // namespace

HaulState HaulCycleSimulator::haulStateAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t < kLoadingSeconds) {
        return HaulState::Loading;
    }
    if (t < kLoadingSeconds + kHaulingSeconds) {
        return HaulState::Hauling;
    }
    if (t < kLoadingSeconds + kHaulingSeconds + kDumpingSeconds) {
        return HaulState::Dumping;
    }
    return HaulState::Returning;
}

double HaulCycleSimulator::payloadAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t < kLoadingOverfillEndSeconds) {
        // The shovel operator slightly overfills the tray before it is trimmed
        // back to the rated capacity, giving a believable brief overload spike.
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

double HaulCycleSimulator::engineTempAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t < kLoadingSeconds) {
        return 70.0 + 15.0 * (t / kLoadingSeconds);
    }
    if (t < kLoadingSeconds + kHaulingSeconds) {
        const double th = (t - kLoadingSeconds) / kHaulingSeconds;
        return 85.0 + 23.0 * th;
    }
    if (t < kLoadingSeconds + kHaulingSeconds + kDumpingSeconds) {
        const double td = (t - (kLoadingSeconds + kHaulingSeconds)) / kDumpingSeconds;
        return 108.0 - 4.0 * td;
    }
    const double tr = (t - (kLoadingSeconds + kHaulingSeconds + kDumpingSeconds)) / kReturningSeconds;
    return 104.0 - 34.0 * tr;
}

double HaulCycleSimulator::retarderTempAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    constexpr double kDescentStartSeconds = kLoadingSeconds + kHaulingSeconds - 20.0;
    const double haulingEnd = kLoadingSeconds + kHaulingSeconds;
    const double dumpingEnd = haulingEnd + kDumpingSeconds;

    if (t < kDescentStartSeconds) {
        return 40.0;
    }
    if (t < haulingEnd) {
        // Descending the haul road into the dump point: the retarder does the
        // braking instead of the service brakes, so its temperature climbs.
        const double td = (t - kDescentStartSeconds) / (haulingEnd - kDescentStartSeconds);
        return 40.0 + 55.0 * td;
    }
    if (t < dumpingEnd) {
        const double td = (t - haulingEnd) / kDumpingSeconds;
        return 95.0 - 10.0 * td;
    }
    const double tr = (t - dumpingEnd) / kReturningSeconds;
    return 85.0 - 45.0 * tr;
}

double HaulCycleSimulator::tyrePressureAt(int wheelIndex, double payloadTonnes)
{
    constexpr double kBasePressureKPa = 700.0;
    constexpr double kMaxLoadInfluenceKPa = 30.0;

    const double loadFraction = std::clamp(payloadTonnes / kRatedCapacityTonnes, 0.0, 1.05);
    const double offset = kWheelPressureOffsetsKPa[static_cast<std::size_t>(wheelIndex) % kWheelPressureOffsetsKPa.size()];
    return kBasePressureKPa + offset + kMaxLoadInfluenceKPa * loadFraction;
}

double HaulCycleSimulator::tyreTempAt(int wheelIndex, double payloadTonnes)
{
    // Simplified quasi-static thermal model: heavier payload flexes the
    // carcass more and drives a faster heat-generation rate, so tyre
    // temperature tracks current payload the same way tyre pressure does.
    // The rear duals (indices 2-5) carry more of the load than the front
    // steer tyres (0-1) and so run hotter for the same payload.
    constexpr double kAmbientTempC = 55.0;
    constexpr double kFrontPeakRiseC = 25.0;
    constexpr double kRearPeakRiseC = 40.0;

    const double loadFraction = std::clamp(payloadTonnes / kRatedCapacityTonnes, 0.0, 1.05);
    const bool isRearWheel = (static_cast<std::size_t>(wheelIndex) % kWheelPressureOffsetsKPa.size()) >= 2;
    const double peakRiseC = isRearWheel ? kRearPeakRiseC : kFrontPeakRiseC;
    return kAmbientTempC + peakRiseC * loadFraction;
}

double HaulCycleSimulator::speedKphAt(double phaseSeconds)
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
    constexpr double kReturningApproachLeadSeconds = 15.0;
    constexpr double kReturningApproachSpeedKph = 5.0;

    const double haulingStart = kLoadingSeconds;
    const double haulingEnd = kLoadingSeconds + kHaulingSeconds;
    const double dumpingEnd = haulingEnd + kDumpingSeconds;

    if (t < kLoadingSeconds) {
        // The truck pulls forward under the shovel then stops to be loaded.
        const double half = kLoadingSeconds / 2.0;
        return t < half ? kLoadingCreepMaxKph * (t / half) : kLoadingCreepMaxKph * (1.0 - (t - half) / half);
    }
    if (t < haulingEnd) {
        const double th = t - haulingStart;
        const double descentStart = kHaulingSeconds - kDescentLeadSeconds;
        if (th < kHaulingAccelEndSeconds) {
            return kHaulingCruiseKph * (th / kHaulingAccelEndSeconds);
        }
        if (th < descentStart) {
            return kHaulingCruiseKph;
        }
        // Descending the haul road into the dump point: the retarder sheds
        // speed ahead of the dump face.
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
    if (tr > kReturningSeconds - kReturningApproachLeadSeconds) {
        const double ta = (tr - (kReturningSeconds - kReturningApproachLeadSeconds)) / kReturningApproachLeadSeconds;
        return kReturningCruiseKph - (kReturningCruiseKph - kReturningApproachSpeedKph) * ta;
    }
    return kReturningCruiseKph;
}

double HaulCycleSimulator::fuelLtrPerHourAt(double payloadTonnes, double speedKph)
{
    // A heavier payload makes the engine work harder to maintain speed, and a
    // higher road speed burns more fuel overcoming rolling/aerodynamic
    // resistance, so fuel consumption is correlated with both rather than an
    // independent random signal.
    constexpr double kIdleFuelLtrPerHour = 45.0;
    constexpr double kLoadFuelRangeLtrPerHour = 90.0;
    constexpr double kSpeedFuelRangeLtrPerHour = 40.0;
    constexpr double kReferenceMaxSpeedKph = 55.0;

    const double loadFraction = std::clamp(payloadTonnes / kRatedCapacityTonnes, 0.0, 1.05);
    const double speedFraction = std::clamp(speedKph / kReferenceMaxSpeedKph, 0.0, 1.2);
    return kIdleFuelLtrPerHour + kLoadFuelRangeLtrPerHour * loadFraction + kSpeedFuelRangeLtrPerHour * speedFraction;
}

SpeedRangeKph HaulCycleSimulator::expectedSpeedRangeAt(HaulState state)
{
    switch (state) {
    case HaulState::Loading:
        return {0.0, 10.0};
    case HaulState::Hauling:
        return {20.0, 50.0};
    case HaulState::Dumping:
        return {0.0, 25.0};
    case HaulState::Returning:
        return {0.0, 60.0};
    }
    return {0.0, 0.0};
}

bool HaulCycleSimulator::isOverloaded(double payloadTonnes)
{
    return payloadTonnes > kRatedCapacityTonnes;
}

HaulTruckState HaulCycleSimulator::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;
    const double phase = std::fmod(m_elapsedSeconds, kCyclePeriodSeconds);

    m_state.haulState = haulStateAt(phase);
    m_state.payloadTonnes = payloadAt(phase);
    m_state.engineTempC = engineTempAt(phase);
    m_state.retarderTempC = retarderTempAt(phase);
    m_state.speedKph = speedKphAt(phase);
    m_state.fuelLtrPerHour = fuelLtrPerHourAt(m_state.payloadTonnes, m_state.speedKph);
    m_state.overloaded = isOverloaded(m_state.payloadTonnes);

    for (std::size_t wheel = 0; wheel < m_state.tyrePressuresKPa.size(); ++wheel) {
        m_state.tyrePressuresKPa[wheel] = tyrePressureAt(static_cast<int>(wheel), m_state.payloadTonnes);
        m_state.tyreTempsC[wheel] = tyreTempAt(static_cast<int>(wheel), m_state.payloadTonnes);
    }

    return m_state;
}

} // namespace qttutorial::mining
