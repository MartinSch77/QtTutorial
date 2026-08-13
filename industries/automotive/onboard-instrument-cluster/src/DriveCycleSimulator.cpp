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

double DriveCycleSimulator::speedTimeConstantSecondsForMode(DrivingMode mode)
{
    switch (mode) {
    case DrivingMode::Eco:
        return 3.0; // gentlest throttle mapping: slow to chase the target speed
    case DrivingMode::Sport:
        return 1.0; // sharpest throttle mapping: snaps to the target speed
    case DrivingMode::Comfort:
    default:
        return 2.0;
    }
}

double DriveCycleSimulator::fuelRateMultiplierForMode(DrivingMode mode)
{
    switch (mode) {
    case DrivingMode::Eco:
        return 0.75;
    case DrivingMode::Sport:
        return 1.45;
    case DrivingMode::Comfort:
    default:
        return 1.0;
    }
}

double DriveCycleSimulator::efficiencyPercentFor(DrivingMode mode, double speedDeltaKph)
{
    const double baseEfficiency = mode == DrivingMode::Eco ? 92.0 : mode == DrivingMode::Sport ? 48.0 : 72.0;
    // Hard acceleration/braking (a large per-tick change in speed) costs efficiency
    // regardless of mode, which is what keeps the number feeling driven rather than
    // a static mode label.
    const double accelPenalty = std::min(std::abs(speedDeltaKph) * 6.0, baseEfficiency);
    return std::clamp(baseEfficiency - accelPenalty, 0.0, 100.0);
}

double DriveCycleSimulator::tirePressureKpaAt(int wheelIndex, double elapsedSeconds)
{
    // Each wheel leaks at a different, deterministic rate (front-left leaks fastest,
    // as if it had picked up a slow puncture) and wraps back to a full 220 kPa once
    // it bottoms out, the same "wrap instead of stalling the demo" convention the
    // fuel model uses so a low-pressure warning is reachable without running for hours.
    static constexpr std::array<double, kTireCount> kLeakKpaPerMinute = {0.9, 0.35, 0.3, 0.32};
    constexpr double kFullPressureKpa = 220.0;
    constexpr double kFloorPressureKpa = 150.0;
    const double index = static_cast<std::size_t>(wheelIndex) < kLeakKpaPerMinute.size()
        ? kLeakKpaPerMinute[static_cast<std::size_t>(wheelIndex)]
        : kLeakKpaPerMinute[0];
    const double elapsedMinutes = elapsedSeconds / 60.0;
    const double range = kFullPressureKpa - kFloorPressureKpa;
    const double drained = std::fmod(elapsedMinutes * index, range);
    return kFullPressureKpa - drained;
}

bool DriveCycleSimulator::isTirePressureLow(double kPa)
{
    return kPa < 180.0;
}

VehicleState DriveCycleSimulator::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;
    const double phase = std::fmod(m_elapsedSeconds, kCyclePeriodSeconds);

    const double previousSpeed = m_state.speedKph;
    const double target = targetSpeedAt(phase);
    const double speedTimeConstantSeconds = speedTimeConstantSecondsForMode(m_drivingMode);
    const double alpha = 1.0 - std::exp(-dtSeconds / speedTimeConstantSeconds);
    m_state.speedKph += (target - m_state.speedKph) * alpha;
    const double speedDeltaKph = m_state.speedKph - previousSpeed;

    const double distanceDeltaKm = std::abs(m_state.speedKph) * dtSeconds / 3600.0;
    m_distanceKm += distanceDeltaKm;
    constexpr double kFuelPercentPerKm = 0.08;
    m_state.fuelLevelPercent -= distanceDeltaKm * kFuelPercentPerKm * fuelRateMultiplierForMode(m_drivingMode);
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

    m_state.drivingMode = m_drivingMode;
    m_state.efficiencyPercent = efficiencyPercentFor(m_drivingMode, speedDeltaKph);

    m_state.tirePressureWarning = false;
    m_state.lowTireWheelIndex = -1;
    for (int wheel = 0; wheel < kTireCount; ++wheel) {
        const double pressure = tirePressureKpaAt(wheel, m_elapsedSeconds);
        m_state.tirePressureKpa[static_cast<std::size_t>(wheel)] = pressure;
        if (isTirePressureLow(pressure) && m_state.lowTireWheelIndex == -1) {
            m_state.tirePressureWarning = true;
            m_state.lowTireWheelIndex = wheel;
        }
    }

    return m_state;
}

} // namespace qttutorial::automotive
