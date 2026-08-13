// SPDX-License-Identifier: MIT
#include "RemoteHomeController.h"

#include "SnapshotDiff.h"

#include <QDateTime>
#include <QHostAddress>

namespace qttutorial::homeautomation::remote {

RemoteHomeController::RemoteHomeController(QObject* parent)
    : QObject(parent)
    , m_activityStore(QStringLiteral(":memory:"), QStringLiteral("offboard_remote_activity"))
{
    connect(&m_client, &HomeLinkClient::snapshotReceived, this, &RemoteHomeController::handleSnapshot);
    connect(&m_client, &HomeLinkClient::connectionStateChanged, this, [this](bool connected) {
        m_connected = connected;
        emit connectedChanged();
    });

    // Self-contained demo: listen on an OS-assigned local port and connect
    // our own client to it. A real deployment would instead point the
    // client at the home hub's actual network address.
    m_server.listen(QHostAddress::LocalHost);
    m_client.connectToHome(QStringLiteral("127.0.0.1"), m_server.serverPort());
}

QVariantList RemoteHomeController::rooms() const
{
    QVariantList list;
    for (const RoomState& room : m_latest.rooms) {
        QVariantMap map;
        map[QStringLiteral("name")] = room.name;
        map[QStringLiteral("lightOn")] = room.lightOn;
        map[QStringLiteral("brightness")] = room.brightness;
        map[QStringLiteral("blindPosition")] = room.blindPosition;
        list << map;
    }
    return list;
}

QVariantList RemoteHomeController::locks() const
{
    QVariantList list;
    for (const LockState& lock : m_latest.locks) {
        QVariantMap map;
        map[QStringLiteral("name")] = lock.name;
        map[QStringLiteral("locked")] = lock.locked;
        list << map;
    }
    return list;
}

QVariantList RemoteHomeController::sensors() const
{
    QVariantList list;
    for (const SensorState& sensor : m_latest.sensors) {
        QVariantMap map;
        map[QStringLiteral("name")] = sensor.name;
        map[QStringLiteral("triggered")] = sensor.triggered;
        list << map;
    }
    return list;
}

QStringList RemoteHomeController::activityLog() const
{
    QStringList lines;
    for (const ActivityLogEntry& entry : m_activityStore.recentEntries(20)) {
        const QString time = QDateTime::fromMSecsSinceEpoch(entry.timestampMs).toString(QStringLiteral("hh:mm:ss"));
        lines << QStringLiteral("%1 at %2").arg(entry.description, time);
    }
    return lines;
}

void RemoteHomeController::setLightOn(const QString& room, bool on)
{
    m_client.sendCommand(Command{.type = QStringLiteral("setLight"), .target = room, .value = on ? 1.0 : 0.0});
}

void RemoteHomeController::setBrightness(const QString& room, int brightness)
{
    m_client.sendCommand(Command{.type = QStringLiteral("setBrightness"), .target = room, .value = static_cast<double>(brightness)});
}

void RemoteHomeController::setBlindPosition(const QString& room, int position)
{
    m_client.sendCommand(Command{.type = QStringLiteral("setBlind"), .target = room, .value = static_cast<double>(position)});
}

void RemoteHomeController::setLockLocked(const QString& lockName, bool locked)
{
    m_client.sendCommand(Command{.type = QStringLiteral("setLock"), .target = lockName, .value = locked ? 1.0 : 0.0});
}

void RemoteHomeController::setArmed(bool armed)
{
    m_client.sendCommand(Command{.type = QStringLiteral("setArmed"), .value = armed ? 1.0 : 0.0});
}

void RemoteHomeController::setThermostatTarget(double celsius)
{
    m_client.sendCommand(Command{.type = QStringLiteral("setThermostatTarget"), .value = celsius});
}

void RemoteHomeController::setThermostatMode(int mode)
{
    m_client.sendCommand(Command{.type = QStringLiteral("setThermostatMode"), .value = static_cast<double>(mode)});
}

void RemoteHomeController::handleSnapshot(const Snapshot& snapshot)
{
    const QStringList changes = describeChanges(m_latest, snapshot);
    m_latest = snapshot;

    if (!changes.isEmpty()) {
        const qint64 now = QDateTime::currentMSecsSinceEpoch();
        for (const QString& change : changes) {
            m_activityStore.record(change, now);
        }
        emit activityLogChanged();
    }

    emit stateChanged();
}

} // namespace qttutorial::homeautomation::remote
