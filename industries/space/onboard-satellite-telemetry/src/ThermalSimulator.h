// SPDX-License-Identifier: MIT
#pragma once

#include <array>

namespace qttutorial::space {

enum class ThermalZone { Radiator, BatteryBay, Payload, Avionics, Count };

// Four thermal zones, pure C++23: each drifts, with its own thermal mass (time
// constant), toward a target temperature that depends on solar input — hot side
// in sunlight, cold side in eclipse. Consistent with the eclipse cycle driving
// the rest of the onboard telemetry.
class ThermalSimulator {
public:
    ThermalSimulator();

    void step(double dtSeconds, double solarInputFraction);

    [[nodiscard]] double temperatureC(ThermalZone zone) const;

private:
    std::array<double, static_cast<std::size_t>(ThermalZone::Count)> m_temperaturesC;
};

} // namespace qttutorial::space
