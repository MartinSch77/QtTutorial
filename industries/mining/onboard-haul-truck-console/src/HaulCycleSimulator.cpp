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
    m_state.overloaded = isOverloaded(m_state.payloadTonnes);

    for (std::size_t wheel = 0; wheel < m_state.tyrePressuresKPa.size(); ++wheel) {
        m_state.tyrePressuresKPa[wheel] = tyrePressureAt(static_cast<int>(wheel), m_state.payloadTonnes);
    }

    return m_state;
}

} // namespace qttutorial::mining
