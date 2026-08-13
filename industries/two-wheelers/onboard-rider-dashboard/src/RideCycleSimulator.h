// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::two_wheelers {

enum class Gear { Neutral, Gear1, Gear2, Gear3, Gear4, Gear5, Gear6 };

// Rider-selectable power-delivery mode, the same genre of control found on modern
// sport-bike TFT dashes (e.g. rain/road/sport/race style riding-mode maps): it does
// not change *where* the bike is in the ride cycle, only how eagerly it gets there
// and how much of the raw cornering lean is let through before the simulated rider
// backs off.
enum class RidingMode { Rain, Road, Sport, Race };

struct RideState {
    static constexpr double kFuelTankCapacityLitres = 15.0;

    double speedKph = 0.0;
    double rpm = 1200.0;
    Gear gear = Gear::Neutral;
    double leanAngleDeg = 0.0; // negative = leaning left, positive = leaning right
    double frontTyrePressureBar = 2.5;
    double rearTyrePressureBar = 2.9;
    double frontTyreTempC = 20.0;
    double rearTyreTempC = 20.0;
    RidingMode mode = RidingMode::Road;
    double fuelLitres = kFuelTankCapacityLitres;
};

// Drives a repeating, deterministic ride cycle (accelerate, cruise, brake into a
// corner, take a chicane-style corner, accelerate out, brake to a stop) with no
// randomness so results are reproducible in tests. Speed follows the profile with
// first-order lag (a motorcycle cannot teleport to a new speed), gear and rpm
// are both derived from that same speed signal, and lean angle is derived from the
// cornering phase of the cycle *and* the current speed within it, which is what
// keeps it correlated with cornering rather than being independent noise. Tyre
// temperature drifts toward a speed-dependent equilibrium with its own thermal lag,
// and tyre pressure is a direct function of that temperature.
//
// The rider-selected RidingMode modulates two further, physically-motivated
// couplings: the time constant with which speed chases its target (Race mode
// "opens the throttle" faster than Rain mode - a steeper simulated power-delivery
// curve), and how much of the raw cornering lean angle is actually let through
// (Rain mode caps lean for a cautious/limited-grip ride, Race mode allows the full
// lean). Fuel burn is derived from rpm (a proxy for throttle opening) and scaled by
// mode, so higher-rpm, higher-mode riding drains the tank faster - fuel is not
// independent noise either.
class RideCycleSimulator {
public:
    static constexpr double kCyclePeriodSeconds = 60.0;
    static constexpr double kCorneringStartSeconds = 30.0;
    static constexpr double kCorneringDurationSeconds = 15.0;

    RideCycleSimulator() = default;

    RideState advance(double dtSeconds);

    [[nodiscard]] const RideState& state() const { return m_state; }

    void setRidingMode(RidingMode mode) { m_state.mode = mode; }
    [[nodiscard]] RidingMode ridingMode() const { return m_state.mode; }

    [[nodiscard]] static double targetSpeedAt(double phaseSeconds);
    [[nodiscard]] static Gear gearForSpeed(double speedKph);
    [[nodiscard]] static double rpmForSpeedAndGear(double speedKph, Gear gear);
    [[nodiscard]] static bool isCorneringPhase(double phaseSeconds);
    [[nodiscard]] static double leanAngleAt(double phaseSeconds, double speedKph);
    [[nodiscard]] static double targetTyreTempAt(double speedKph, bool isRearTyre);
    [[nodiscard]] static double tyrePressureForTemp(double tyreTempC, bool isRearTyre);

    // Power-delivery curve: how quickly (in seconds) speed chases its target for a
    // given mode. Smaller = snappier throttle response.
    [[nodiscard]] static double speedTimeConstantSecondsForMode(RidingMode mode);
    // Fraction (0-1] of the raw cornering lean angle that is let through for a
    // given mode; lower in Rain to model a more conservative rider/traction limit.
    [[nodiscard]] static double leanAngleFactorForMode(RidingMode mode);
    // Fuel consumption rate in litres/hour for the given engine speed and mode.
    [[nodiscard]] static double fuelBurnRateLitresPerHour(double rpm, RidingMode mode);

private:
    double m_elapsedSeconds = 0.0;
    RideState m_state;
};

[[nodiscard]] const char* ridingModeLabel(RidingMode mode);

} // namespace qttutorial::two_wheelers
