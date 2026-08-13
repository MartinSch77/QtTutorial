// SPDX-License-Identifier: MIT
#pragma once

#include "AlertLevel.h"

namespace qttutorial::startrek_bridge {

// Drives the Engineering station's ship-interior readouts: warp-core power
// output, hull integrity and internal temperature. Power output eases
// toward a higher target under Red alert (all power to the warp core);
// hull integrity very slowly erodes under sustained Red alert and recovers
// under Green, staying within a plausible band rather than jumping. Pure
// C++23, deterministic given elapsed time and the current AlertLevel.
class EngineeringSimulator {
public:
    void advance(double dtSeconds, AlertLevel alertLevel);

    [[nodiscard]] double powerOutputPercent() const { return m_powerOutputPercent; }
    [[nodiscard]] double hullIntegrityPercent() const { return m_hullIntegrityPercent; }
    [[nodiscard]] double internalTempC() const { return m_internalTempC; }

private:
    double m_powerOutputPercent = 62.0;
    double m_hullIntegrityPercent = 100.0;
    double m_internalTempC = 21.0;
    double m_elapsedSeconds = 0.0;
};

} // namespace qttutorial::startrek_bridge
