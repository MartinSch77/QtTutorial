// SPDX-License-Identifier: MIT
#include "EchoServer.h"

#include "Protocol.h"

#include <QDateTime>
#include <QTcpSocket>

namespace qttutorial::networking {

EchoServer::EchoServer(QObject* parent)
    : QTcpServer(parent)
{
    connect(this, &QTcpServer::newConnection, this, &EchoServer::handleNewConnection);
}

void EchoServer::handleNewConnection()
{
    while (hasPendingConnections()) {
        QTcpSocket* socket = nextPendingConnection();
        m_buffers.insert(socket, QByteArray());
        connect(socket, &QTcpSocket::readyRead, this, &EchoServer::handleReadyRead);
        connect(socket, &QTcpSocket::disconnected, this, &EchoServer::handleClientDisconnected);
    }
}

void EchoServer::handleReadyRead()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }

    QByteArray& buffer = m_buffers[socket];
    buffer += socket->readAll();

    qsizetype newlineIndex = 0;
    while ((newlineIndex = buffer.indexOf('\n')) != -1) {
        const QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);

        const std::optional<Request> request = decodeRequest(line);
        if (!request) {
            continue;
        }

        Response response;
        response.id = request->id;
        response.clientTimestampMs = request->clientTimestampMs;
        response.serverTimestampMs = QDateTime::currentMSecsSinceEpoch();
        response.echo = QStringLiteral("echo of request %1").arg(request->id);

        socket->write(encodeResponse(response));
    }
}

void EchoServer::handleClientDisconnected()
{
    auto* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        return;
    }
    m_buffers.remove(socket);
    socket->deleteLater();
}

} // namespace qttutorial::networking
