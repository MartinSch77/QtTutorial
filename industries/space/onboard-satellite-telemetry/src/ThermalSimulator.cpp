// SPDX-License-Identifier: MIT
#include "ThermalSimulator.h"

#include <algorithm>

namespace qttutorial::space {

namespace {
struct ZoneProfile {
    double coldTempC;
    double hotTempC;
    double timeConstantS;
};

constexpr std::array<ZoneProfile, static_cast<std::size_t>(ThermalZone::Count)> kProfiles{{
    {-40.0, 10.0, 400.0},  // Radiator: fast, wide swing
    {5.0, 25.0, 900.0},    // BatteryBay: kept in a narrow, warm band
    {-20.0, 35.0, 600.0},  // Payload
    {0.0, 30.0, 700.0},    // Avionics
}};
}

ThermalSimulator::ThermalSimulator()
{
    for (std::size_t i = 0; i < m_temperaturesC.size(); ++i) {
        m_temperaturesC[i] = (kProfiles[i].coldTempC + kProfiles[i].hotTempC) / 2.0;
    }
}

void ThermalSimulator::step(double dtSeconds, double solarInputFraction)
{
    for (std::size_t i = 0; i < m_temperaturesC.size(); ++i) {
        const ZoneProfile& profile = kProfiles[i];
        const double target = profile.coldTempC + solarInputFraction * (profile.hotTempC - profile.coldTempC);
        const double alpha = std::clamp(dtSeconds / profile.timeConstantS, 0.0, 1.0);
        m_temperaturesC[i] += (target - m_temperaturesC[i]) * alpha;
    }
}

double ThermalSimulator::temperatureC(ThermalZone zone) const
{
    return m_temperaturesC[static_cast<std::size_t>(zone)];
}

} // namespace qttutorial::space
