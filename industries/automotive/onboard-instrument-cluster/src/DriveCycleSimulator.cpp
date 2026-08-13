// SPDX-License-Identifier: MIT
#include "DriveCycleSimulator.h"

#include <algorithm>
#include <cmath>
#include <numbers>

namespace qttutorial::automotive {

namespace {

constexpr double kTwoPi = 2.0 * std::numbers::pi;

}

double DriveCycleSimulator::targetSpeedAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t < 15.0) {
        return 100.0 * (t / 15.0);
    }
    if (t < 35.0) {
        return 100.0 + 5.0 * std::sin(kTwoPi * (t - 15.0) / 8.0);
    }
    if (t < 45.0) {
        return 100.0 * (1.0 - (t - 35.0) / 10.0);
    }
    if (t < 50.0) {
        return 0.0;
    }
    if (t < 55.0) {
        return -10.0 * ((t - 50.0) / 5.0);
    }
    return -10.0 * (1.0 - (t - 55.0) / 5.0);
}

Gear DriveCycleSimulator::gearForSpeed(double speedKph)
{
    if (speedKph < -0.5) {
        return Gear::Reverse;
    }
    if (speedKph < 0.5) {
        return Gear::Park;
    }
    if (speedKph < 20.0) {
        return Gear::Drive1;
    }
    if (speedKph < 40.0) {
        return Gear::Drive2;
    }
    if (speedKph < 60.0) {
        return Gear::Drive3;
    }
    if (speedKph < 80.0) {
        return Gear::Drive4;
    }
    if (speedKph < 100.0) {
        return Gear::Drive5;
    }
    return Gear::Drive6;
}

double DriveCycleSimulator::rpmForSpeedAndGear(double speedKph, Gear gear)
{
    constexpr double kIdleRpm = 800.0;
    constexpr double kMaxRpm = 6000.0;

    if (gear == Gear::Park) {
        return kIdleRpm;
    }
    if (gear == Gear::Reverse) {
        return std::clamp(900.0 + std::abs(speedKph) * 20.0, 900.0, 2500.0);
    }

    static constexpr double kGearMinSpeed[] = {0.0, 20.0, 40.0, 60.0, 80.0, 100.0};
    const auto gearIndex = static_cast<std::size_t>(gear) - static_cast<std::size_t>(Gear::Drive1);
    const double minSpeed = kGearMinSpeed[gearIndex];
    return std::clamp(900.0 + (speedKph - minSpeed) * 30.0, 900.0, kMaxRpm);
}

int DriveCycleSimulator::followingDistanceAt(double phaseSeconds)
{
    const double t = std::fmod(phaseSeconds, kCyclePeriodSeconds);

    if (t >= 30.0 && t < 40.0) {
        const double closing = 3.0 * (1.0 - (t - 30.0) / 10.0);
        return static_cast<int>(std::clamp(std::round(closing), 0.0, 3.0));
    }
    if (t >= 40.0 && t < 50.0) {
        const double opening = 3.0 * ((t - 40.0) / 10.0);
        return static_cast<int>(std::clamp(std::round(opening), 0.0, 3.0));
    }
    return 3;
}

bool DriveCycleSimulator::isLowFuel(double fuelLevelPercent)
{
    return fuelLevelPercent < 15.0;
}

VehicleState DriveCycleSimulator::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;
    const double phase = std::fmod(m_elapsedSeconds, kCyclePeriodSeconds);

    const double target = targetSpeedAt(phase);
    constexpr double kSpeedTimeConstantSeconds = 2.0;
    const double alpha = 1.0 - std::exp(-dtSeconds / kSpeedTimeConstantSeconds);
    m_state.speedKph += (target - m_state.speedKph) * alpha;

    const double distanceDeltaKm = std::abs(m_state.speedKph) * dtSeconds / 3600.0;
    m_distanceKm += distanceDeltaKm;
    constexpr double kFuelPercentPerKm = 0.08;
    m_state.fuelLevelPercent -= distanceDeltaKm * kFuelPercentPerKm;
    if (m_state.fuelLevelPercent <= 0.0) {
        m_state.fuelLevelPercent = 100.0;
    }
    m_state.lowFuelWarning = isLowFuel(m_state.fuelLevelPercent);

    m_state.gear = gearForSpeed(m_state.speedKph);
    m_state.rpm = rpmForSpeedAndGear(m_state.speedKph, m_state.gear);

    m_state.followingDistanceLevel = followingDistanceAt(phase);
    m_state.laneWarning = m_state.followingDistanceLevel == 0;

    const bool blinkOn = std::fmod(m_elapsedSeconds, 0.6) < 0.3;
    m_state.leftTurnSignal = blinkOn && phase >= 10.0 && phase < 14.0;
    m_state.rightTurnSignal = blinkOn && phase >= 46.0 && phase < 50.0;

    return m_state;
}

} // namespace qttutorial::automotive
