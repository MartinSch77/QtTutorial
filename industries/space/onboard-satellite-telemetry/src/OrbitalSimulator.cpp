// SPDX-License-Identifier: MIT
#include "OrbitalSimulator.h"

#include <algorithm>
#include <cmath>

namespace qttutorial::space {

OrbitalSimulator::OrbitalSimulator(double eclipseFractionOfOrbit)
    : m_eclipseFractionOfOrbit(eclipseFractionOfOrbit)
{
}

void OrbitalSimulator::step(double dtSeconds)
{
    const double periodSeconds = m_state.orbitalPeriodMinutes * 60.0;
    const double degreesPerSecond = 360.0 / periodSeconds;
    m_state.trueAnomalyDeg = std::fmod(m_state.trueAnomalyDeg + degreesPerSecond * dtSeconds, 360.0);

    const double eclipseHalfWidthDeg = m_eclipseFractionOfOrbit * 180.0;
    const double distanceFromEclipseCenter = std::abs(std::fmod(m_state.trueAnomalyDeg - 180.0 + 540.0, 360.0) - 180.0);
    m_state.inEclipse = distanceFromEclipseCenter < eclipseHalfWidthDeg;

    if (m_state.inEclipse) {
        m_state.solarInputFraction = 0.0;
    } else {
        const double fadeZoneDeg = 5.0;
        const double distanceFromEdge = distanceFromEclipseCenter - eclipseHalfWidthDeg;
        m_state.solarInputFraction = std::clamp(distanceFromEdge / fadeZoneDeg, 0.0, 1.0);
    }
}

} // namespace qttutorial::space
