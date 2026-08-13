// SPDX-License-Identifier: MIT
#include "Protocol.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

namespace qttutorial::homeautomation::remote {

namespace {

QByteArray frame(const QJsonObject& object)
{
    return QJsonDocument(object).toJson(QJsonDocument::Compact) + '\n';
}

QJsonObject roomToJson(const RoomState& room)
{
    QJsonObject object;
    object[QStringLiteral("name")] = room.name;
    object[QStringLiteral("lightOn")] = room.lightOn;
    object[QStringLiteral("brightness")] = room.brightness;
    object[QStringLiteral("blindPosition")] = room.blindPosition;
    return object;
}

RoomState roomFromJson(const QJsonObject& object)
{
    RoomState room;
    room.name = object.value(QStringLiteral("name")).toString();
    room.lightOn = object.value(QStringLiteral("lightOn")).toBool();
    room.brightness = object.value(QStringLiteral("brightness")).toInt();
    room.blindPosition = object.value(QStringLiteral("blindPosition")).toInt();
    return room;
}

QJsonObject lockToJson(const LockState& lock)
{
    QJsonObject object;
    object[QStringLiteral("name")] = lock.name;
    object[QStringLiteral("locked")] = lock.locked;
    return object;
}

LockState lockFromJson(const QJsonObject& object)
{
    LockState lock;
    lock.name = object.value(QStringLiteral("name")).toString();
    lock.locked = object.value(QStringLiteral("locked")).toBool();
    return lock;
}

QJsonObject sensorToJson(const SensorState& sensor)
{
    QJsonObject object;
    object[QStringLiteral("name")] = sensor.name;
    object[QStringLiteral("triggered")] = sensor.triggered;
    return object;
}

SensorState sensorFromJson(const QJsonObject& object)
{
    SensorState sensor;
    sensor.name = object.value(QStringLiteral("name")).toString();
    sensor.triggered = object.value(QStringLiteral("triggered")).toBool();
    return sensor;
}

} // namespace

QByteArray encodeCommand(const Command& command)
{
    QJsonObject object;
    object[QStringLiteral("type")] = command.type;
    object[QStringLiteral("target")] = command.target;
    object[QStringLiteral("value")] = command.value;
    return frame(object);
}

std::optional<Command> decodeCommand(const QByteArray& line)
{
    const QJsonDocument doc = QJsonDocument::fromJson(line);
    if (!doc.isObject()) {
        return std::nullopt;
    }
    const QJsonObject object = doc.object();
    if (!object.contains(QStringLiteral("type"))) {
        return std::nullopt;
    }

    Command command;
    command.type = object.value(QStringLiteral("type")).toString();
    command.target = object.value(QStringLiteral("target")).toString();
    command.value = object.value(QStringLiteral("value")).toDouble();
    return command;
}

QByteArray encodeSnapshot(const Snapshot& snapshot)
{
    QJsonArray rooms;
    for (const RoomState& room : snapshot.rooms) {
        rooms.append(roomToJson(room));
    }
    QJsonArray locks;
    for (const LockState& lock : snapshot.locks) {
        locks.append(lockToJson(lock));
    }
    QJsonArray sensors;
    for (const SensorState& sensor : snapshot.sensors) {
        sensors.append(sensorToJson(sensor));
    }

    QJsonObject object;
    object[QStringLiteral("rooms")] = rooms;
    object[QStringLiteral("locks")] = locks;
    object[QStringLiteral("armed")] = snapshot.armed;
    object[QStringLiteral("sensors")] = sensors;
    object[QStringLiteral("thermostatMode")] = snapshot.thermostatMode;
    object[QStringLiteral("thermostatCurrent")] = snapshot.thermostatCurrent;
    object[QStringLiteral("thermostatTarget")] = snapshot.thermostatTarget;
    return frame(object);
}

std::optional<Snapshot> decodeSnapshot(const QByteArray& line)
{
    const QJsonDocument doc = QJsonDocument::fromJson(line);
    if (!doc.isObject()) {
        return std::nullopt;
    }
    const QJsonObject object = doc.object();
    if (!object.contains(QStringLiteral("rooms")) || !object.contains(QStringLiteral("locks"))) {
        return std::nullopt;
    }

    Snapshot snapshot;
    for (const QJsonValue& value : object.value(QStringLiteral("rooms")).toArray()) {
        snapshot.rooms.push_back(roomFromJson(value.toObject()));
    }
    for (const QJsonValue& value : object.value(QStringLiteral("locks")).toArray()) {
        snapshot.locks.push_back(lockFromJson(value.toObject()));
    }
    snapshot.armed = object.value(QStringLiteral("armed")).toBool();
    for (const QJsonValue& value : object.value(QStringLiteral("sensors")).toArray()) {
        snapshot.sensors.push_back(sensorFromJson(value.toObject()));
    }
    snapshot.thermostatMode = object.value(QStringLiteral("thermostatMode")).toInt();
    snapshot.thermostatCurrent = object.value(QStringLiteral("thermostatCurrent")).toDouble();
    snapshot.thermostatTarget = object.value(QStringLiteral("thermostatTarget")).toDouble();
    return snapshot;
}

} // namespace qttutorial::homeautomation::remote
