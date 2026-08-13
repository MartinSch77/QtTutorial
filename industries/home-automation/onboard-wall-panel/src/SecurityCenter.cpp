// SPDX-License-Identifier: MIT
#include "SecurityCenter.h"

#include <algorithm>

namespace qttutorial::homeautomation {

SecurityCenter::SecurityCenter(std::vector<Sensor> sensors)
    : m_sensors(std::move(sensors))
{
}

bool SecurityCenter::isValidIndex(int index) const
{
    return index >= 0 && index < sensorCount();
}

void SecurityCenter::setSensorTriggered(int index, bool triggered)
{
    if (!isValidIndex(index)) {
        return;
    }
    m_sensors[static_cast<std::size_t>(index)].triggered = triggered;
}

bool SecurityCenter::isBreach() const
{
    if (!m_armed) {
        return false;
    }
    return std::any_of(m_sensors.begin(), m_sensors.end(), [](const Sensor& sensor) { return sensor.triggered; });
}

std::vector<Sensor> SecurityCenter::defaultSensors()
{
    return {
        Sensor{.name = QStringLiteral("Front Door")},
        Sensor{.name = QStringLiteral("Back Door")},
        Sensor{.name = QStringLiteral("Living Room Window")},
        Sensor{.name = QStringLiteral("Hallway Motion")},
    };
}

} // namespace qttutorial::homeautomation
