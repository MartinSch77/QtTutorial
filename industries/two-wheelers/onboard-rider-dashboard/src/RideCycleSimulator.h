// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::two_wheelers {

enum class Gear { Neutral, Gear1, Gear2, Gear3, Gear4, Gear5, Gear6 };

struct RideState {
    double speedKph = 0.0;
    double rpm = 1200.0;
    Gear gear = Gear::Neutral;
    double leanAngleDeg = 0.0; // negative = leaning left, positive = leaning right
    double frontTyrePressureBar = 2.5;
    double rearTyrePressureBar = 2.9;
    double frontTyreTempC = 20.0;
    double rearTyreTempC = 20.0;
};

// Drives a repeating, deterministic ride cycle (accelerate, cruise, brake into a
// corner, take a chicane-style corner, accelerate out, brake to a stop) with no
// randomness so results are reproducible in tests. Speed follows the profile with
// first-order lag (a motorcycle cannot teleport to its target speed), gear and rpm
// are both derived from that same speed signal, and lean angle is derived from the
// cornering phase of the cycle *and* the current speed within it, which is what
// keeps it correlated with cornering rather than being independent noise. Tyre
// temperature drifts toward a speed-dependent equilibrium with its own thermal lag,
// and tyre pressure is a direct function of that temperature.
class RideCycleSimulator {
public:
    static constexpr double kCyclePeriodSeconds = 60.0;
    static constexpr double kCorneringStartSeconds = 30.0;
    static constexpr double kCorneringDurationSeconds = 15.0;

    RideCycleSimulator() = default;

    RideState advance(double dtSeconds);

    [[nodiscard]] const RideState& state() const { return m_state; }

    [[nodiscard]] static double targetSpeedAt(double phaseSeconds);
    [[nodiscard]] static Gear gearForSpeed(double speedKph);
    [[nodiscard]] static double rpmForSpeedAndGear(double speedKph, Gear gear);
    [[nodiscard]] static bool isCorneringPhase(double phaseSeconds);
    [[nodiscard]] static double leanAngleAt(double phaseSeconds, double speedKph);
    [[nodiscard]] static double targetTyreTempAt(double speedKph, bool isRearTyre);
    [[nodiscard]] static double tyrePressureForTemp(double tyreTempC, bool isRearTyre);

private:
    double m_elapsedSeconds = 0.0;
    RideState m_state;
};

} // namespace qttutorial::two_wheelers
