// SPDX-License-Identifier: MIT
#include "HudSimulator.h"

#include <cmath>

namespace qttutorial::defence {

namespace {
double normalizeDeg(double deg)
{
    double result = std::fmod(deg, 360.0);
    if (result < 0.0) {
        result += 360.0;
    }
    return result;
}
}

HudSimulator::HudSimulator()
    : m_waypoints{
          {QStringLiteral("RALLY-1"), 30.0, 1.2},
          {QStringLiteral("RALLY-2"), 150.0, 2.4},
      }
    , m_teammates{
          {1, 10.0, 0.4},
          {2, 300.0, 0.7},
          {3, 200.0, 0.9},
      }
{
}

void HudSimulator::advance(double dtSeconds)
{
    m_elapsedSeconds += dtSeconds;

    m_headingDeg = normalizeDeg(m_headingDeg + 4.0 * dtSeconds);

    for (std::size_t i = 0; i < m_teammates.size(); ++i) {
        HudTeammate& teammate = m_teammates[i];
        const double phase = m_elapsedSeconds * 0.15 + static_cast<double>(i) * 2.0;
        teammate.bearingDeg = normalizeDeg(teammate.bearingDeg + std::sin(phase) * 0.5 * dtSeconds);
        teammate.distanceKm = 0.5 + 0.3 * std::sin(phase * 0.5);
    }
}

} // namespace qttutorial::defence
