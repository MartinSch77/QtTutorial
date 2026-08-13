// SPDX-License-Identifier: MIT
#pragma once

namespace qttutorial::homeautomation {

// Pure simulated thermal model for the wall panel's thermostat: no Qt GUI or
// timer dependency, so the rate/mode behaviour can be unit tested step by
// step. advance() is meant to be called repeatedly with a real elapsed time
// in seconds (an app-level QTimer façade owns the actual clock).
class ThermostatModel {
public:
    enum class Mode { Off, Heat, Cool };

    // Degrees Celsius per second the system can move the room temperature
    // when actively heating or cooling towards the target.
    static constexpr double kHeatCoolRatePerSecond = 1.0;
    // Degrees Celsius per second the room drifts towards the outside
    // ambient temperature when the system is off - deliberately slower than
    // kHeatCoolRatePerSecond since this is passive heat loss/gain, not an
    // active system.
    static constexpr double kDriftRatePerSecond = 0.15;

    ThermostatModel();

    void setMode(Mode mode) { m_mode = mode; }
    [[nodiscard]] Mode mode() const { return m_mode; }

    void setTargetTemperature(double celsius) { m_targetTemperature = celsius; }
    [[nodiscard]] double targetTemperature() const { return m_targetTemperature; }

    void setOutsideAmbient(double celsius) { m_outsideAmbient = celsius; }
    [[nodiscard]] double outsideAmbient() const { return m_outsideAmbient; }

    void setCurrentTemperature(double celsius) { m_currentTemperature = celsius; }
    [[nodiscard]] double currentTemperature() const { return m_currentTemperature; }

    // Moves currentTemperature() one simulation step of deltaSeconds towards
    // its drive target: the set target when actively heating/cooling (and
    // only while the room is on the wrong side of that target - a heater
    // never actively cools, and vice versa), or the outside ambient when the
    // system is off.
    void advance(double deltaSeconds);

private:
    Mode m_mode = Mode::Off;
    double m_currentTemperature = 19.0;
    double m_targetTemperature = 21.0;
    double m_outsideAmbient = 8.0;
};

} // namespace qttutorial::homeautomation
