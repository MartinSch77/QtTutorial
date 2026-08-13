// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::homeautomation {

// A simulated door/window/motion sensor. `triggered` means "open" for a
// door/window sensor and "motion detected" for a motion sensor - the wall
// panel just needs a name and a boolean to render a status tile.
struct Sensor {
    QString name;
    bool triggered = false;
};

// Tracks the armed/disarmed state of the security system and its sensors,
// and decides whether the current combination counts as a breach. No Qt GUI
// dependency, so it is unit testable independent of the QML wall panel.
class SecurityCenter {
public:
    explicit SecurityCenter(std::vector<Sensor> sensors = defaultSensors());

    [[nodiscard]] bool armed() const { return m_armed; }
    void setArmed(bool armed) { m_armed = armed; }

    [[nodiscard]] int sensorCount() const { return static_cast<int>(m_sensors.size()); }
    [[nodiscard]] const Sensor& sensor(int index) const { return m_sensors.at(static_cast<std::size_t>(index)); }
    void setSensorTriggered(int index, bool triggered);

    // A breach only exists while armed - an open door with the system
    // disarmed is just normal household activity, not an alarm condition.
    [[nodiscard]] bool isBreach() const;

    [[nodiscard]] static std::vector<Sensor> defaultSensors();

private:
    [[nodiscard]] bool isValidIndex(int index) const;

    bool m_armed = false;
    std::vector<Sensor> m_sensors;
};

} // namespace qttutorial::homeautomation
