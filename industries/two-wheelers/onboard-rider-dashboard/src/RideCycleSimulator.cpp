// SPDX-License-Identifier: MIT
#include "RideCycleSimulator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::two_wheelers {

namespace {

constexpr double kTwoPi = 2.0 * std::numbers::pi;
constexpr double kCorneringNominalSpeedKph = 45.0;
constexpr double kMaxLeanAngleDeg = 38.0;

}

double RideCycleSimulator::targetSpeedAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t < 10.0) {
        return 90.0 * (t / 10.0);
    }
    if (t < 20.0) {
        return 90.0 + 4.0 * std::sin(kTwoPi * (t - 10.0) / 6.0);
    }
    if (t < kCorneringStartSeconds) {
        return 90.0 - 45.0 * ((t - 20.0) / 10.0);
    }
    if (t < kCorneringStartSeconds + kCorneringDurationSeconds) {
        return kCorneringNominalSpeedKph
            + 3.0 * std::sin(kTwoPi * (t - kCorneringStartSeconds) / kCorneringDurationSeconds);
    }
    if (t < 55.0) {
        return kCorneringNominalSpeedKph + 45.0 * ((t - 45.0) / 10.0);
    }
    return 90.0 * (1.0 - (t - 55.0) / 5.0);
}

Gear RideCycleSimulator::gearForSpeed(double speedKph)
{
    if (speedKph < 0.5) {
        return Gear::Neutral;
    }
    if (speedKph < 15.0) {
        return Gear::Gear1;
    }
    if (speedKph < 30.0) {
        return Gear::Gear2;
    }
    if (speedKph < 45.0) {
        return Gear::Gear3;
    }
    if (speedKph < 60.0) {
        return Gear::Gear4;
    }
    if (speedKph < 75.0) {
        return Gear::Gear5;
    }
    return Gear::Gear6;
}

double RideCycleSimulator::rpmForSpeedAndGear(double speedKph, Gear gear)
{
    constexpr double kIdleRpm = 1200.0;
    constexpr double kMaxRpm = 11000.0;

    if (gear == Gear::Neutral) {
        return kIdleRpm;
    }

    static constexpr double kGearMinSpeed[] = {0.0, 15.0, 30.0, 45.0, 60.0, 75.0};
    const auto gearIndex = static_cast<std::size_t>(gear) - static_cast<std::size_t>(Gear::Gear1);
    const double minSpeed = kGearMinSpeed[gearIndex];
    return std::clamp(kIdleRpm + (speedKph - minSpeed) * 180.0, kIdleRpm, kMaxRpm);
}

bool RideCycleSimulator::isCorneringPhase(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);
    return t >= kCorneringStartSeconds && t < kCorneringStartSeconds + kCorneringDurationSeconds;
}

double RideCycleSimulator::leanAngleAt(double phaseSeconds, double speedKph)
{
    if (!isCorneringPhase(phaseSeconds)) {
        return 0.0;
    }
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds) - kCorneringStartSeconds;
    const double turnRate = std::sin(kTwoPi * t / kCorneringDurationSeconds);
    const double speedFactor = std::clamp(speedKph / kCorneringNominalSpeedKph, 0.3, 1.3);
    return kMaxLeanAngleDeg * turnRate * speedFactor;
}

double RideCycleSimulator::targetTyreTempAt(double speedKph, bool isRearTyre)
{
    constexpr double kAmbientTempC = 25.0;
    constexpr double kRearExtraHeatC = 8.0;
    const double target = kAmbientTempC + speedKph * 0.35;
    return isRearTyre ? target + kRearExtraHeatC : target;
}

double RideCycleSimulator::tyrePressureForTemp(double tyreTempC, bool isRearTyre)
{
    constexpr double kBaselineTempC = 25.0;
    constexpr double kPressurePerDegree = 0.012;
    const double basePressure = isRearTyre ? 2.9 : 2.5;
    return basePressure + (tyreTempC - kBaselineTempC) * kPressurePerDegree;
}

RideState RideCycleSimulator::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;
    const double phase = std::fmod(m_elapsedSeconds, kCyclePeriodSeconds);

    const double targetSpeed = targetSpeedAt(phase);
    constexpr double kSpeedTimeConstantSeconds = 1.5;
    const double speedAlpha = 1.0 - std::exp(-dtSeconds / kSpeedTimeConstantSeconds);
    m_state.speedKph += (targetSpeed - m_state.speedKph) * speedAlpha;

    m_state.gear = gearForSpeed(m_state.speedKph);
    m_state.rpm = rpmForSpeedAndGear(m_state.speedKph, m_state.gear);

    const double targetLean = leanAngleAt(phase, m_state.speedKph);
    constexpr double kLeanTimeConstantSeconds = 0.8;
    const double leanAlpha = 1.0 - std::exp(-dtSeconds / kLeanTimeConstantSeconds);
    m_state.leanAngleDeg += (targetLean - m_state.leanAngleDeg) * leanAlpha;

    constexpr double kThermalTimeConstantSeconds = 25.0;
    const double thermalAlpha = 1.0 - std::exp(-dtSeconds / kThermalTimeConstantSeconds);
    const double targetFrontTemp = targetTyreTempAt(m_state.speedKph, false);
    const double targetRearTemp = targetTyreTempAt(m_state.speedKph, true);
    m_state.frontTyreTempC += (targetFrontTemp - m_state.frontTyreTempC) * thermalAlpha;
    m_state.rearTyreTempC += (targetRearTemp - m_state.rearTyreTempC) * thermalAlpha;

    m_state.frontTyrePressureBar = tyrePressureForTemp(m_state.frontTyreTempC, false);
    m_state.rearTyrePressureBar = tyrePressureForTemp(m_state.rearTyreTempC, true);

    return m_state;
}

} // namespace qttutorial::two_wheelers
