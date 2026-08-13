// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::space {

struct PowerState {
    double batterySocPercent = 85.0;
    double solarPanelOutputWatts = 0.0;
    double busLoadWatts = 150.0;
};

// Battery state-of-charge, pure C++23: charges only when there is solar input
// (i.e. not in eclipse) and the panels are sun-pointed, discharges continuously
// against a constant bus load. Internally consistent by construction: the
// eclipse/sun-pointing inputs are the only things that can make the battery
// charge, everything else just drains it.
class PowerSystem {
public:
    void step(double dtSeconds, double solarInputFraction, bool panelsSunPointed);

    [[nodiscard]] const PowerState& state() const { return m_state; }

private:
    static constexpr double kMaxPanelOutputWatts = 320.0;
    static constexpr double kBatteryCapacityWattHours = 120.0;

    PowerState m_state;
};

} // namespace qttutorial::space
