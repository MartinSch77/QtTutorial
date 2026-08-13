// SPDX-License-Identifier: MIT
#pragma once

#include <QByteArray>
#include <QString>

#include <optional>
#include <vector>

namespace qttutorial::homeautomation::remote {

struct RoomState {
    QString name;
    bool lightOn = false;
    int brightness = 0;
    int blindPosition = 0;
};

struct LockState {
    QString name;
    bool locked = true;
};

struct SensorState {
    QString name;
    bool triggered = false;
};

// A full picture of the home's state, pushed from the home system to a
// remote client whenever anything changes.
struct Snapshot {
    std::vector<RoomState> rooms;
    std::vector<LockState> locks;
    bool armed = false;
    std::vector<SensorState> sensors;
    int thermostatMode = 0; // matches ThermostatController::Mode: Off=0, Heat=1, Cool=2
    double thermostatCurrent = 20.0;
    double thermostatTarget = 21.0;
};

// A control request sent from a remote client to the home system. `target`
// names the room/lock affected (empty when not applicable, e.g. setArmed);
// `value` carries a brightness/position/temperature, or 0/1 for a boolean.
struct Command {
    QString type;
    QString target;
    double value = 0.0;
};

// One JSON object per line, the same simple framing used in
// framework-tour/04-networking/src/Protocol.h: easy to split a raw
// QTcpSocket byte stream back into messages without a length prefix.
[[nodiscard]] QByteArray encodeCommand(const Command& command);
[[nodiscard]] std::optional<Command> decodeCommand(const QByteArray& line);

[[nodiscard]] QByteArray encodeSnapshot(const Snapshot& snapshot);
[[nodiscard]] std::optional<Snapshot> decodeSnapshot(const QByteArray& line);

} // namespace qttutorial::homeautomation::remote
