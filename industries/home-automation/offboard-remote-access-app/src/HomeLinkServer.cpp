// SPDX-License-Identifier: MIT
#include "HomeLinkServer.h"

#include <QTcpSocket>

namespace qttutorial::homeautomation::remote {

namespace {
constexpr int kSensorBlipIntervalMs = 4000;
}

HomeLinkServer::HomeLinkServer(QObject* parent)
    : QTcpServer(parent)
{
    connect(this, &QTcpServer::newConnection, this, &HomeLinkServer::handleNewConnection);

    m_sensorTimer.setInterval(kSensorBlipIntervalMs);
    connect(&m_sensorTimer, &QTimer::timeout, this, &HomeLinkServer::simulateSensorBlip);
    m_sensorTimer.start();
}

void HomeLinkServer::handleNewConnection()
{
    while (hasPendingConnections()) {
        QTcpSocket* socket = nextPendingConnection();
        m_buffers.insert(socket, QByteArray());
        connect(socket, &QTcpSocket::readyRead, this, &HomeLinkServer::handleReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &HomeLinkServer::handleClientDisconnected);
        socket->write(encodeSnapshot(m_simulator.snapshot()));
    }
}

void HomeLinkServer::handleReadyRead()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QByteArray& buffer = m_buffers[socket];
    buffer += socket->readAll();

    bool changed = false;
    qsizetype newlineIndex = 0;
    while ((newlineIndex = buffer.indexOf('\n')) != -1) {
        const QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);

        const std::optional<Command> command = decodeCommand(line);
        if (command && m_simulator.applyCommand(*command)) {
            changed = true;
        }
    }

    if (changed) {
        broadcastSnapshot();
    }
}

void HomeLinkServer::handleClientDisconnected()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    m_buffers.remove(socket);
    socket->deleteLater();
}

void HomeLinkServer::simulateSensorBlip()
{
    if (m_simulator.snapshot().sensors.empty()) {
        return;
    }

    if (m_activeSensor != -1) {
        m_simulator.setSensorTriggered(m_activeSensor, false);
        m_activeSensor = -1;
    }

    ++m_tick;
    // Every fifth tick stays quiet so the sensor grid does not feel like it
    // is triggering constantly.
    if (m_tick % 5 != 4) {
        m_activeSensor = m_tick % static_cast<int>(m_simulator.snapshot().sensors.size());
        m_simulator.setSensorTriggered(m_activeSensor, true);
    }

    broadcastSnapshot();
}

void HomeLinkServer::broadcastSnapshot()
{
    const QByteArray payload = encodeSnapshot(m_simulator.snapshot());
    for (QTcpSocket* socket : m_buffers.keys()) {
        socket->write(payload);
    }
}

} // namespace qttutorial::homeautomation::remote
