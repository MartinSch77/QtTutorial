// SPDX-License-Identifier: MIT
#pragma once

#include <array>

namespace qttutorial::automotive {

enum class Gear { Park, Reverse, Drive1, Drive2, Drive3, Drive4, Drive5, Drive6 };

// Driving-mode selector, akin to the Eco/Comfort/Sport modes found on modern digital
// instrument clusters: it retunes how eagerly the simulated drivetrain responds to
// the same target-speed profile and how that translates into simulated efficiency.
enum class DrivingMode { Eco, Comfort, Sport };

inline constexpr int kTireCount = 4;

struct VehicleState {
    double speedKph = 0.0;
    double rpm = 800.0;
    Gear gear = Gear::Park;
    double fuelLevelPercent = 100.0;
    bool leftTurnSignal = false;
    bool rightTurnSignal = false;
    int followingDistanceLevel = 3; // 0 = closing in on traffic ahead, 3 = clear road
    bool laneWarning = false;
    bool lowFuelWarning = false;
    DrivingMode drivingMode = DrivingMode::Comfort;
    double efficiencyPercent = 70.0; // 0-100 "how efficiently is the car being driven right now"
    std::array<double, kTireCount> tirePressureKpa = {220.0, 220.0, 220.0, 220.0};
    bool tirePressureWarning = false;
    int lowTireWheelIndex = -1; // -1 = all tires within range
};

// Drives a repeating, physically plausible cycle (accelerate, cruise, brake for
// following traffic, stop, reverse) with no randomness so results are reproducible
// in tests. Speed follows the profile with first-order lag (a car cannot teleport
// to its target speed), and rpm/gear/fuel/warnings are all derived from that single
// speed signal, which is what keeps them correlated the way a real drivetrain is.
//
// The driving mode (Eco/Comfort/Sport) retunes the same underlying target-speed
// profile: Sport mode chases the target speed more aggressively (shorter time
// constant) and burns more fuel per km travelled, while Eco mode responds more
// gently and sips fuel; efficiencyPercent is derived from both the active mode and
// the instantaneous acceleration demand, so hard acceleration visibly hurts
// efficiency even in Eco mode, and gentle driving in Sport mode still under-performs
// Eco. Tire pressure slowly drifts down per-wheel (a different, deterministic leak
// rate per wheel) and wraps around after triggering a low-pressure warning, the same
// "wrap instead of running out" convention the fuel model already uses so the demo
// reaches the interesting state without running for hours.
class DriveCycleSimulator {
public:
    static constexpr double kCyclePeriodSeconds = 60.0;

    DriveCycleSimulator() = default;

    VehicleState advance(double dtSeconds);

    [[nodiscard]] const VehicleState& state() const { return m_state; }

    void setDrivingMode(DrivingMode mode) { m_drivingMode = mode; }
    [[nodiscard]] DrivingMode drivingMode() const { return m_drivingMode; }

    [[nodiscard]] static double targetSpeedAt(double phaseSeconds);
    [[nodiscard]] static Gear gearForSpeed(double speedKph);
    [[nodiscard]] static double rpmForSpeedAndGear(double speedKph, Gear gear);
    [[nodiscard]] static int followingDistanceAt(double phaseSeconds);
    [[nodiscard]] static bool isLowFuel(double fuelLevelPercent);

    [[nodiscard]] static double speedTimeConstantSecondsForMode(DrivingMode mode);
    [[nodiscard]] static double fuelRateMultiplierForMode(DrivingMode mode);
    [[nodiscard]] static double efficiencyPercentFor(DrivingMode mode, double speedDeltaKph);
    [[nodiscard]] static double tirePressureKpaAt(int wheelIndex, double elapsedSeconds);
    [[nodiscard]] static bool isTirePressureLow(double kPa);

private:
    double m_elapsedSeconds = 0.0;
    double m_distanceKm = 0.0;
    DrivingMode m_drivingMode = DrivingMode::Comfort;
    VehicleState m_state;
};

} // namespace qttutorial::automotive
