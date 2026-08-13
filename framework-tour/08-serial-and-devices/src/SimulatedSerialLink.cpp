// SPDX-License-Identifier: MIT
#include "SimulatedSerialLink.h"

#include <QLocalServer>
#include <QLocalSocket>
#include <QUuid>

namespace qttutorial::serial_and_devices {

SimulatedSerialLink::SimulatedSerialLink(QObject* parent)
    : QObject(parent)
    , m_server(new QLocalServer(this))
    , m_clientSocket(new QLocalSocket(this))
{
    const QString serverName = QStringLiteral("qttutorial-serial-sim-%1")
                                    .arg(QUuid::createUuid().toString(QUuid::Id128));

    // Stale sockets from a previous crashed run can leave the name behind on Unix.
    QLocalServer::removeServer(serverName);
    m_server->listen(serverName);

    m_clientSocket->connectToServer(serverName);

    // Local socket connection setup is asynchronous even on loopback, so pump the
    // event loop briefly for both the server's accept and the client's connect.
    m_server->waitForNewConnection(1000);
    m_deviceSocket = m_server->nextPendingConnection();
    if (m_deviceSocket) {
        m_deviceSocket->setParent(this);
    }
    m_clientSocket->waitForConnected(1000);
}

SimulatedSerialLink::~SimulatedSerialLink() = default;

bool SimulatedSerialLink::isConnected() const
{
    return m_deviceSocket != nullptr && m_clientSocket->state() == QLocalSocket::ConnectedState
        && m_deviceSocket->state() == QLocalSocket::ConnectedState;
}

} // namespace qttutorial::serial_and_devices
