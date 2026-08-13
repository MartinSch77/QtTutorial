// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::avionics {

struct ControlInput {
    double elevator = 0.0; // [-1, 1], positive pitches nose up
    double aileron = 0.0;  // [-1, 1], positive rolls right
    double throttle = 0.5; // [0, 1]
};

struct FlightState {
    double pitchDeg = 0.0;
    double rollDeg = 0.0;
    double headingDeg = 0.0;
    double altitudeFt = 5000.0;
    double verticalSpeedFtPerMin = 0.0;
    double airspeedKt = 220.0;
};

// A deliberately simplified, coupled flight-dynamics model: control inputs drive
// damped first-order responses in roll and pitch, and those in turn drive heading,
// vertical speed and airspeed, so every instrument on the PFD moves consistently
// rather than jittering independently. Not aerodynamically accurate, but the
// couplings (bank => turn, pitch => climb/descend => speed bleed) are directionally
// correct, which is what a demo audience notices.
class FlightDynamics {
public:
    void setControlInput(const ControlInput& input);
    void step(double dtSeconds);

    [[nodiscard]] const FlightState& state() const { return m_state; }

private:
    FlightState m_state;
    ControlInput m_input;
};

} // namespace qttutorial::avionics
