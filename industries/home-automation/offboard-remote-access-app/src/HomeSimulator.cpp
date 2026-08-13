// SPDX-License-Identifier: MIT
#include "HomeSimulator.h"

#include <algorithm>

namespace qttutorial::homeautomation::remote {

HomeSimulator::HomeSimulator()
{
    m_snapshot.rooms = {
        RoomState{.name = QStringLiteral("Living Room"), .lightOn = true, .brightness = 80, .blindPosition = 40},
        RoomState{.name = QStringLiteral("Kitchen"), .lightOn = true, .brightness = 100, .blindPosition = 100},
        RoomState{.name = QStringLiteral("Bedroom"), .lightOn = false, .brightness = 30, .blindPosition = 10},
        RoomState{.name = QStringLiteral("Bathroom"), .lightOn = false, .brightness = 60, .blindPosition = 100},
        RoomState{.name = QStringLiteral("Hallway"), .lightOn = true, .brightness = 50, .blindPosition = 0},
    };
    m_snapshot.locks = {
        LockState{.name = QStringLiteral("Front Door"), .locked = true},
        LockState{.name = QStringLiteral("Back Door"), .locked = true},
        LockState{.name = QStringLiteral("Garage Door"), .locked = false},
    };
    m_snapshot.sensors = {
        SensorState{.name = QStringLiteral("Front Door")},
        SensorState{.name = QStringLiteral("Back Door")},
        SensorState{.name = QStringLiteral("Living Room Window")},
        SensorState{.name = QStringLiteral("Hallway Motion")},
    };
    m_snapshot.armed = false;
    m_snapshot.thermostatMode = 1;
    m_snapshot.thermostatCurrent = 19.0;
    m_snapshot.thermostatTarget = 21.0;
}

int HomeSimulator::roomIndex(const QString& name) const
{
    const auto it = std::find_if(m_snapshot.rooms.begin(), m_snapshot.rooms.end(),
                                  [&name](const RoomState& room) { return room.name == name; });
    return it == m_snapshot.rooms.end() ? -1 : static_cast<int>(std::distance(m_snapshot.rooms.begin(), it));
}

int HomeSimulator::lockIndex(const QString& name) const
{
    const auto it = std::find_if(m_snapshot.locks.begin(), m_snapshot.locks.end(),
                                  [&name](const LockState& lock) { return lock.name == name; });
    return it == m_snapshot.locks.end() ? -1 : static_cast<int>(std::distance(m_snapshot.locks.begin(), it));
}

void HomeSimulator::setSensorTriggered(int index, bool triggered)
{
    if (index < 0 || static_cast<std::size_t>(index) >= m_snapshot.sensors.size()) {
        return;
    }
    m_snapshot.sensors[static_cast<std::size_t>(index)].triggered = triggered;
}

bool HomeSimulator::applyCommand(const Command& command)
{
    if (command.type == QStringLiteral("setLight")) {
        const int index = roomIndex(command.target);
        if (index < 0) {
            return false;
        }
        const bool on = command.value != 0.0;
        if (m_snapshot.rooms[static_cast<std::size_t>(index)].lightOn == on) {
            return false;
        }
        m_snapshot.rooms[static_cast<std::size_t>(index)].lightOn = on;
        return true;
    }
    if (command.type == QStringLiteral("setBrightness")) {
        const int index = roomIndex(command.target);
        if (index < 0) {
            return false;
        }
        const int value = std::clamp(static_cast<int>(command.value), 0, 100);
        if (m_snapshot.rooms[static_cast<std::size_t>(index)].brightness == value) {
            return false;
        }
        m_snapshot.rooms[static_cast<std::size_t>(index)].brightness = value;
        return true;
    }
    if (command.type == QStringLiteral("setBlind")) {
        const int index = roomIndex(command.target);
        if (index < 0) {
            return false;
        }
        const int value = std::clamp(static_cast<int>(command.value), 0, 100);
        if (m_snapshot.rooms[static_cast<std::size_t>(index)].blindPosition == value) {
            return false;
        }
        m_snapshot.rooms[static_cast<std::size_t>(index)].blindPosition = value;
        return true;
    }
    if (command.type == QStringLiteral("setLock")) {
        const int index = lockIndex(command.target);
        if (index < 0) {
            return false;
        }
        const bool locked = command.value != 0.0;
        if (m_snapshot.locks[static_cast<std::size_t>(index)].locked == locked) {
            return false;
        }
        m_snapshot.locks[static_cast<std::size_t>(index)].locked = locked;
        return true;
    }
    if (command.type == QStringLiteral("setArmed")) {
        const bool armed = command.value != 0.0;
        if (m_snapshot.armed == armed) {
            return false;
        }
        m_snapshot.armed = armed;
        return true;
    }
    if (command.type == QStringLiteral("setThermostatTarget")) {
        if (m_snapshot.thermostatTarget == command.value) {
            return false;
        }
        m_snapshot.thermostatTarget = command.value;
        return true;
    }
    if (command.type == QStringLiteral("setThermostatMode")) {
        const int mode = static_cast<int>(command.value);
        if (m_snapshot.thermostatMode == mode) {
            return false;
        }
        m_snapshot.thermostatMode = mode;
        return true;
    }
    return false;
}

} // namespace qttutorial::homeautomation::remote
