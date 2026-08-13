// SPDX-License-Identifier: MIT
#pragma once

#include <array>

namespace qttutorial::mining {

enum class HaulState { Loading, Hauling, Dumping, Returning };

struct HaulTruckState {
    HaulState haulState = HaulState::Loading;
    double payloadTonnes = 0.0;
    double engineTempC = 70.0;
    double retarderTempC = 40.0;
    double speedKph = 0.0;
    double fuelLtrPerHour = 0.0;
    std::array<double, 6> tyrePressuresKPa{};
    std::array<double, 6> tyreTempsC{};
    bool overloaded = false;
};

// Plausible ground speed envelope for a given haul-cycle phase, used to flag a
// truck running outside its expected performance envelope for that phase
// (e.g. crawling while "hauling" would indicate a bogged-down or faulted
// truck) without needing a full physics model.
struct SpeedRangeKph {
    double minKph = 0.0;
    double maxKph = 0.0;
};

// Drives a repeating, physically plausible haul cycle (load, haul, dump, return)
// with no randomness so results are reproducible in tests. Payload, engine
// temperature, retarder temperature, and tyre pressures are all derived from a
// single elapsed-time signal, which is what keeps them correlated the way a
// real haul truck's systems are: the engine works hardest hauling a full load,
// the retarder gets hot braking down into the dump point, and tyre pressure
// creeps up slightly under a heavier payload.
class HaulCycleSimulator {
public:
    static constexpr double kLoadingSeconds = 30.0;
    static constexpr double kHaulingSeconds = 90.0;
    static constexpr double kDumpingSeconds = 15.0;
    static constexpr double kReturningSeconds = 60.0;
    static constexpr double kCyclePeriodSeconds =
        kLoadingSeconds + kHaulingSeconds + kDumpingSeconds + kReturningSeconds;

    static constexpr double kRatedCapacityTonnes = 300.0;
    static constexpr double kPeakLoadingOverfillTonnes = 315.0;

    HaulCycleSimulator() = default;

    HaulTruckState advance(double dtSeconds);

    [[nodiscard]] const HaulTruckState& state() const { return m_state; }

    [[nodiscard]] static HaulState haulStateAt(double phaseSeconds);
    [[nodiscard]] static double payloadAt(double phaseSeconds);
    [[nodiscard]] static double engineTempAt(double phaseSeconds);
    [[nodiscard]] static double retarderTempAt(double phaseSeconds);
    [[nodiscard]] static double speedKphAt(double phaseSeconds);
    [[nodiscard]] static double fuelLtrPerHourAt(double payloadTonnes, double speedKph);
    [[nodiscard]] static double tyrePressureAt(int wheelIndex, double payloadTonnes);
    [[nodiscard]] static double tyreTempAt(int wheelIndex, double payloadTonnes);
    [[nodiscard]] static SpeedRangeKph expectedSpeedRangeAt(HaulState state);
    [[nodiscard]] static bool isOverloaded(double payloadTonnes);

private:
    double m_elapsedSeconds = 0.0;
    HaulTruckState m_state;
};

} // namespace qttutorial::mining
