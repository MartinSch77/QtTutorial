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
    // A simplified engine-gas-temperature-style parameter: it rises toward a
    // target set by throttle demand (plus a small climb-power penalty, since a
    // sustained climb asks the engine for more than level flight would) and
    // relaxes back down on its own thermal time constant. It is deliberately
    // coupled to the same control inputs as everything else so a sustained
    // high-throttle climb is what actually drives it toward the caution band,
    // rather than it being independent random noise.
    double engineTempC = 400.0;
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

    // Nominal engine temperature band; above kEngineTempCautionC for a sustained
    // period is what the caution annunciator watches for.
    static constexpr double kEngineTempNominalC = 400.0;
    static constexpr double kEngineTempCautionC = 650.0;

private:
    FlightState m_state;
    ControlInput m_input;
};

} // namespace qttutorial::avionics
