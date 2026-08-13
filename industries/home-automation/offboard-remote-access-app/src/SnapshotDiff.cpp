// SPDX-License-Identifier: MIT
#include "SnapshotDiff.h"

namespace qttutorial::homeautomation::remote {

namespace {
QString modeName(int mode)
{
    switch (mode) {
    case 1:
        return QStringLiteral("Heat");
    case 2:
        return QStringLiteral("Cool");
    default:
        return QStringLiteral("Off");
    }
}
} // namespace

QStringList describeChanges(const Snapshot& previous, const Snapshot& next)
{
    QStringList changes;

    const std::size_t roomCount = std::min(previous.rooms.size(), next.rooms.size());
    for (std::size_t i = 0; i < roomCount; ++i) {
        const RoomState& before = previous.rooms[i];
        const RoomState& after = next.rooms[i];
        if (before.lightOn != after.lightOn) {
            changes << QStringLiteral("%1 lights turned %2").arg(after.name, after.lightOn ? QStringLiteral("on") : QStringLiteral("off"));
        } else if (after.lightOn && before.brightness != after.brightness) {
            changes << QStringLiteral("%1 brightness set to %2%").arg(after.name).arg(after.brightness);
        }
        if (before.blindPosition != after.blindPosition) {
            changes << QStringLiteral("%1 shades set to %2%").arg(after.name).arg(after.blindPosition);
        }
    }

    const std::size_t lockCount = std::min(previous.locks.size(), next.locks.size());
    for (std::size_t i = 0; i < lockCount; ++i) {
        if (previous.locks[i].locked != next.locks[i].locked) {
            changes << QStringLiteral("%1 %2").arg(next.locks[i].name, next.locks[i].locked ? QStringLiteral("locked") : QStringLiteral("unlocked"));
        }
    }

    if (previous.armed != next.armed) {
        changes << (next.armed ? QStringLiteral("Security system armed") : QStringLiteral("Security system disarmed"));
    }

    if (previous.thermostatMode != next.thermostatMode) {
        changes << QStringLiteral("Thermostat mode set to %1").arg(modeName(next.thermostatMode));
    }
    if (previous.thermostatTarget != next.thermostatTarget) {
        changes << QStringLiteral("Thermostat target set to %1°C").arg(next.thermostatTarget, 0, 'f', 1);
    }

    return changes;
}

} // namespace qttutorial::homeautomation::remote
