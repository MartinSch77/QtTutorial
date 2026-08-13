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
    std::array<double, 6> tyrePressuresKPa{};
    bool overloaded = false;
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
    [[nodiscard]] static double tyrePressureAt(int wheelIndex, double payloadTonnes);
    [[nodiscard]] static bool isOverloaded(double payloadTonnes);

private:
    double m_elapsedSeconds = 0.0;
    HaulTruckState m_state;
};

} // namespace qttutorial::mining
