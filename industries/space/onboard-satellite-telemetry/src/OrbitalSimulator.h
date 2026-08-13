// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::space {

struct OrbitalState {
    double trueAnomalyDeg = 0.0; // position around the orbit, 0 = ascending node
    double altitudeKm = 550.0;
    double orbitalPeriodMinutes = 95.0;
    bool inEclipse = false;
    double solarInputFraction = 1.0; // 0 in full eclipse, 1 in full sun
};

// A simplified circular low-Earth-orbit model, pure C++23: the satellite sweeps
// through true anomaly at a constant rate, and a fixed arc of the orbit
// (centered opposite the sun) is in Earth's shadow. This is what drives the
// eclipse cycle the rest of the onboard telemetry (battery, thermal) reacts to.
class OrbitalSimulator {
public:
    explicit OrbitalSimulator(double eclipseFractionOfOrbit = 0.35);

    void step(double dtSeconds);

    [[nodiscard]] const OrbitalState& state() const { return m_state; }

private:
    OrbitalState m_state;
    double m_eclipseFractionOfOrbit;
};

} // namespace qttutorial::space
