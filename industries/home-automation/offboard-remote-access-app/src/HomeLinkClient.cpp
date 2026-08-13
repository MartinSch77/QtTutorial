// SPDX-License-Identifier: MIT
#include "HomeLinkClient.h"

namespace qttutorial::homeautomation::remote {

HomeLinkClient::HomeLinkClient(QObject* parent)
    : QObject(parent)
{
    connect(&m_socket, &QTcpSocket::readyRead, this, &HomeLinkClient::handleReadyRead);
    connect(&m_socket, &QTcpSocket::connected, this, [this] { emit connectionStateChanged(true); });
    connect(&m_socket, &QTcpSocket::disconnected, this, [this] { emit connectionStateChanged(false); });
}

void HomeLinkClient::connectToHome(const QString& host, quint16 port)
{
    m_socket.connectToHost(host, port);
}

void HomeLinkClient::sendCommand(const Command& command)
{
    if (m_socket.state() != QAbstractSocket::ConnectedState) {
        return;
    }
    m_socket.write(encodeCommand(command));
}

bool HomeLinkClient::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

void HomeLinkClient::handleReadyRead()
{
    m_buffer += m_socket.readAll();

    qsizetype newlineIndex = 0;
    while ((newlineIndex = m_buffer.indexOf('\n')) != -1) {
        const QByteArray line = m_buffer.left(newlineIndex);
        m_buffer.remove(0, newlineIndex + 1);

        const std::optional<Snapshot> snapshot = decodeSnapshot(line);
        if (snapshot) {
            emit snapshotReceived(*snapshot);
        }
    }
}

} // namespace qttutorial::homeautomation::remote
