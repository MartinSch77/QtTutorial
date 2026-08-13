// SPDX-License-Identifier: MIT
#include "VehicleStatusSimulator.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace qttutorial::defence {

namespace {
constexpr std::array<double, 4> kTargetHeadingsDeg = {45.0, 130.0, 220.0, 310.0};
constexpr double kHeadingLegSeconds = 40.0;

double normalizeDeg(double deg)
{
    double result = std::fmod(deg, 360.0);
    if (result < 0.0) {
        result += 360.0;
    }
    return result;
}

double angularDifferenceDeg(double from, double to)
{
    double diff = std::fmod(to - from + 540.0, 360.0) - 180.0;
    return diff;
}
}

VehicleStatusSimulator::VehicleStatusSimulator() = default;

void VehicleStatusSimulator::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;

    const auto legIndex = static_cast<std::size_t>(m_elapsedSeconds / kHeadingLegSeconds) % kTargetHeadingsDeg.size();
    const double targetHeadingDeg = kTargetHeadingsDeg[legIndex];
    const double diff = angularDifferenceDeg(m_status.headingDeg, targetHeadingDeg);
    const double maxStep = kMaxTurnRateDegPerSec * dtSeconds;
    const double step = std::clamp(diff, -maxStep, maxStep);
    m_status.headingDeg = normalizeDeg(m_status.headingDeg + step);

    m_status.speedUnits = 22.0 + 8.0 * std::sin(m_elapsedSeconds * 0.05);

    m_status.fuelPercent = std::max(
        0.0, m_status.fuelPercent - m_status.speedUnits * kFuelConsumptionPerUnitSpeedPerSecond * dtSeconds);
}

} // namespace qttutorial::defence
