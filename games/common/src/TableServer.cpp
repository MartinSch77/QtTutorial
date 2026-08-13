// SPDX-License-Identifier: MIT
#include "TableServer.h"

#include <QTcpSocket>

namespace qttutorial::games::common {

TableServer::TableServer(int maxSeats, QObject* parent)
    : QObject(parent)
    // m_server is a value member, not heap-allocated: it must NOT be given
    // `this` as a QObject parent, or QObject's destructor will try to
    // `delete` a non-heap subobject when TableServer is destroyed.
    , m_server()
    , m_maxSeats(maxSeats)
{
    connect(&m_server, &QTcpServer::newConnection, this, &TableServer::onNewConnection);
}

bool TableServer::listen(quint16 port)
{
    return m_server.listen(QHostAddress::Any, port);
}

quint16 TableServer::serverPort() const
{
    return m_server.serverPort();
}

TableServer::~TableServer()
{
    // QTcpServer::nextPendingConnection() parents accepted sockets to the
    // server, so m_server's own destructor (which runs after this body,
    // when member destruction unwinds) will delete each socket -- and that
    // delete would fire the disconnected-handler lambdas below, which touch
    // m_seatSockets/m_buffers. Those members are declared (and therefore
    // destroyed) before m_server, so without disconnecting first, the
    // handlers would run against already-destroyed containers. Disconnect
    // everything from `this` up front so no signal fires during teardown.
    for (QTcpSocket* socket : std::as_const(m_seatSockets)) {
        socket->disconnect(this);
    }
}

int TableServer::seatCount() const
{
    return static_cast<int>(m_seatSockets.size());
}

void TableServer::onNewConnection()
{
    while (m_server.hasPendingConnections()) {
        QTcpSocket* socket = m_server.nextPendingConnection();
        if (seatCount() >= m_maxSeats) {
            socket->disconnectFromHost();
            socket->deleteLater();
            continue;
        }

        int seat = 0;
        while (m_seatSockets.contains(seat)) {
            ++seat;
        }
        m_seatSockets.insert(seat, socket);
        m_buffers.insert(socket, QByteArray());

        connect(socket, &QTcpSocket::readyRead, this, [this, socket, seat] { onReadyRead(socket, seat); });
        connect(socket, &QTcpSocket::disconnected, this, [this, seat] { onDisconnected(seat); });

        emit seatConnected(seat);
    }
}

void TableServer::onReadyRead(QTcpSocket* socket, int seat)
{
    QByteArray& buffer = m_buffers[socket];
    buffer += socket->readAll();

    qsizetype newlineIndex = buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = buffer.left(newlineIndex);
        buffer.remove(0, newlineIndex + 1);

        if (const auto message = TableMessage::fromLine(line); message.has_value()) {
            emit messageReceived(seat, *message);
        }

        newlineIndex = buffer.indexOf('\n');
    }
}

void TableServer::onDisconnected(int seat)
{
    if (QTcpSocket* socket = m_seatSockets.take(seat)) {
        m_buffers.remove(socket);
        socket->deleteLater();
    }
    emit seatDisconnected(seat);
}

void TableServer::broadcast(const TableMessage& message)
{
    const QByteArray line = message.toLine();
    for (QTcpSocket* socket : std::as_const(m_seatSockets)) {
        socket->write(line);
    }
}

void TableServer::sendTo(int seat, const TableMessage& message)
{
    if (QTcpSocket* socket = m_seatSockets.value(seat)) {
        socket->write(message.toLine());
    }
}

} // namespace qttutorial::games::common
