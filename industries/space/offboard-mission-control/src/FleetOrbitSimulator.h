// SPDX-License-Identifier: MIT
#pragma once

#include "Satellite.h"

#include <vector>

namespace qttutorial::space {

// Advances a small fleet of satellites, each on its own circular orbit phase
// with its own eclipse arc, and integrates each one's battery the same way the
// onboard app does (charge only when sunlit, constant discharge) so the fleet
// table shows internally consistent, not independently-jittering, numbers.
// Pure C++23, no Qt dependency beyond QString already used by Satellite.h.
class FleetOrbitSimulator {
public:
    FleetOrbitSimulator();

    void step(double dtSeconds);

    [[nodiscard]] const std::vector<SatelliteState>& satellites() const { return m_satellites; }

private:
    std::vector<SatelliteState> m_satellites;
    std::vector<double> m_orbitalPeriodMinutes;
    std::vector<double> m_eclipseFraction;
};

} // namespace qttutorial::space
