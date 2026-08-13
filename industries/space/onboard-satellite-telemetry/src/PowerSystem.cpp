// SPDX-License-Identifier: MIT
#include "PowerSystem.h"

#include <algorithm>

namespace qttutorial::space {

void PowerSystem::step(double dtSeconds, double solarInputFraction, bool panelsSunPointed)
{
    m_state.solarPanelOutputWatts = panelsSunPointed ? solarInputFraction * kMaxPanelOutputWatts : 0.0;

    const double netWatts = m_state.solarPanelOutputWatts - m_state.busLoadWatts;
    const double netWattHours = netWatts * (dtSeconds / 3600.0);
    const double socDelta = netWattHours / kBatteryCapacityWattHours * 100.0;

    m_state.batterySocPercent = std::clamp(m_state.batterySocPercent + socDelta, 0.0, 100.0);
}

} // namespace qttutorial::space
