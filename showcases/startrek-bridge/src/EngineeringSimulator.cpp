// SPDX-License-Identifier: MIT
#include "EngineeringSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::startrek_bridge {

namespace {
double clamp(double value, double low, double high)
{
    return std::max(low, std::min(high, value));
}
}

void EngineeringSimulator::advance(double dtSeconds, AlertLevel alertLevel)
{
    m_elapsedSeconds += dtSeconds;

    const double powerTarget = alertLevel == AlertLevel::Red
        ? 92.0
        : (alertLevel == AlertLevel::Yellow ? 74.0 : 62.0) + 6.0 * std::sin(m_elapsedSeconds * 0.07);
    m_powerOutputPercent += (powerTarget - m_powerOutputPercent) * std::min(1.0, dtSeconds * 0.5);
    m_powerOutputPercent = clamp(m_powerOutputPercent, 0.0, 100.0);

    const double stressRate = alertLevel == AlertLevel::Red ? -0.03 : 0.05;
    m_hullIntegrityPercent = clamp(m_hullIntegrityPercent + stressRate * dtSeconds, 85.0, 100.0);

    const double tempTarget = 20.0 + (m_powerOutputPercent - 60.0) * 0.12;
    m_internalTempC += (tempTarget - m_internalTempC) * std::min(1.0, dtSeconds * 0.3);
    m_internalTempC = clamp(m_internalTempC, 17.0, 30.0);
}

} // namespace qttutorial::startrek_bridge
