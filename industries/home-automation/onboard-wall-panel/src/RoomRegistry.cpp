// SPDX-License-Identifier: MIT
#include "RoomRegistry.h"

#include <algorithm>

namespace qttutorial::homeautomation {

RoomRegistry::RoomRegistry(std::vector<Room> rooms)
    : m_rooms(std::move(rooms))
{
}

bool RoomRegistry::isValidIndex(int index) const
{
    return index >= 0 && index < count();
}

void RoomRegistry::setLightOn(int index, bool on)
{
    if (!isValidIndex(index)) {
        return;
    }
    m_rooms[static_cast<std::size_t>(index)].lightOn = on;
}

void RoomRegistry::setBrightness(int index, int brightness)
{
    if (!isValidIndex(index)) {
        return;
    }
    m_rooms[static_cast<std::size_t>(index)].brightness = std::clamp(brightness, 0, 100);
}

void RoomRegistry::setBlindPosition(int index, int position)
{
    if (!isValidIndex(index)) {
        return;
    }
    m_rooms[static_cast<std::size_t>(index)].blindPosition = std::clamp(position, 0, 100);
}

std::vector<Room> RoomRegistry::defaultRooms()
{
    return {
        Room{.name = QStringLiteral("Living Room"), .lightOn = true, .brightness = 80, .blindPosition = 40},
        Room{.name = QStringLiteral("Kitchen"), .lightOn = true, .brightness = 100, .blindPosition = 100},
        Room{.name = QStringLiteral("Bedroom"), .lightOn = false, .brightness = 30, .blindPosition = 10},
        Room{.name = QStringLiteral("Bathroom"), .lightOn = false, .brightness = 60, .blindPosition = 100},
        Room{.name = QStringLiteral("Hallway"), .lightOn = true, .brightness = 50, .blindPosition = 0},
    };
}

} // namespace qttutorial::homeautomation
