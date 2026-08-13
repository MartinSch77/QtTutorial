// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <vector>

namespace qttutorial::homeautomation {

// One room's lighting and blind/shade state. Kept together because the wall
// panel's room selector shows both aspects of a room side by side.
struct Room {
    QString name;
    bool lightOn = false;
    int brightness = 70;
    int blindPosition = 50;
};

// Holds the lighting/blind state for a handful of rooms and the toggling
// logic (including clamping) for them. No Qt GUI dependency, so it is unit
// testable independent of the QML wall panel.
class RoomRegistry {
public:
    explicit RoomRegistry(std::vector<Room> rooms = defaultRooms());

    [[nodiscard]] int count() const { return static_cast<int>(m_rooms.size()); }
    [[nodiscard]] const Room& room(int index) const { return m_rooms.at(static_cast<std::size_t>(index)); }

    void setLightOn(int index, bool on);
    void setBrightness(int index, int brightness);
    void setBlindPosition(int index, int position);

    [[nodiscard]] static std::vector<Room> defaultRooms();

private:
    [[nodiscard]] bool isValidIndex(int index) const;

    std::vector<Room> m_rooms;
};

} // namespace qttutorial::homeautomation
