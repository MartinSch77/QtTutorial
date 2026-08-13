// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::automotive {

enum class Gear { Park, Reverse, Drive1, Drive2, Drive3, Drive4, Drive5, Drive6 };

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
};

// Drives a repeating, physically plausible cycle (accelerate, cruise, brake for
// following traffic, stop, reverse) with no randomness so results are reproducible
// in tests. Speed follows the profile with first-order lag (a car cannot teleport
// to its target speed), and rpm/gear/fuel/warnings are all derived from that single
// speed signal, which is what keeps them correlated the way a real drivetrain is.
class DriveCycleSimulator {
public:
    static constexpr double kCyclePeriodSeconds = 60.0;

    DriveCycleSimulator() = default;

    VehicleState advance(double dtSeconds);

    [[nodiscard]] const VehicleState& state() const { return m_state; }

    [[nodiscard]] static double targetSpeedAt(double phaseSeconds);
    [[nodiscard]] static Gear gearForSpeed(double speedKph);
    [[nodiscard]] static double rpmForSpeedAndGear(double speedKph, Gear gear);
    [[nodiscard]] static int followingDistanceAt(double phaseSeconds);
    [[nodiscard]] static bool isLowFuel(double fuelLevelPercent);

private:
    double m_elapsedSeconds = 0.0;
    double m_distanceKm = 0.0;
    VehicleState m_state;
};

} // namespace qttutorial::automotive
