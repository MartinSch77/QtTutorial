// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::defence {

// Heading/speed/fuel readout for the "vehicle crew display" panel. Stands in
// for a tank/ship/submarine crew station status board.
struct VehicleStatus {
    double headingDeg = 45.0;
    double speedUnits = 20.0;
    double fuelPercent = 100.0;
};

// Pure C++23 kinematic model: heading eases toward a slowly-cycling target
// heading, speed oscillates within a plausible cruising band, and fuel is
// consumed proportionally to speed. Fully deterministic given a fixed
// starting state and elapsed time, so it is unit-testable without Qt.
class VehicleStatusSimulator {
public:
    VehicleStatusSimulator();

    void advance(double dtSeconds);

    [[nodiscard]] const VehicleStatus& status() const { return m_status; }

private:
    static constexpr double kMaxTurnRateDegPerSec = 6.0;
    static constexpr double kFuelConsumptionPerUnitSpeedPerSecond = 0.0025;

    VehicleStatus m_status;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::defence
