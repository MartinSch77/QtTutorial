// SPDX-License-Identifier: MIT
#include "TableClient.h"

namespace qttutorial::games::common {

TableClient::TableClient(QObject* parent)
    : QObject(parent)
    // m_socket is a value member, not heap-allocated: it must NOT be given
    // `this` as a QObject parent (see TableServer.cpp for why).
    , m_socket()
{
    connect(&m_socket, &QTcpSocket::connected, this, &TableClient::connected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &TableClient::disconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &TableClient::onReadyRead);
    connect(&m_socket, &QTcpSocket::errorOccurred, this,
            [this](QAbstractSocket::SocketError) { emit connectionError(m_socket.errorString()); });
}

void TableClient::connectToHost(const QString& host, quint16 port)
{
    m_socket.connectToHost(host, port);
}

void TableClient::disconnectFromHost()
{
    m_socket.disconnectFromHost();
}

bool TableClient::isConnected() const
{
    return m_socket.state() == QAbstractSocket::ConnectedState;
}

void TableClient::send(const TableMessage& message)
{
    m_socket.write(message.toLine());
}

void TableClient::onReadyRead()
{
    m_buffer += m_socket.readAll();

    qsizetype newlineIndex = m_buffer.indexOf('\n');
    while (newlineIndex >= 0) {
        const QByteArray line = m_buffer.left(newlineIndex);
        m_buffer.remove(0, newlineIndex + 1);

        if (const auto message = TableMessage::fromLine(line); message.has_value()) {
            emit messageReceived(*message);
        }

        newlineIndex = m_buffer.indexOf('\n');
    }
}

} // namespace qttutorial::games::common
