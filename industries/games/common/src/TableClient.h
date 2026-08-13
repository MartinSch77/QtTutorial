// SPDX-License-Identifier: MIT
#pragma once

#include "TableMessage.h"

#include <QObject>
#include <QTcpSocket>

namespace qttutorial::games::common {

// The client side of TableServer: connects over TCP, frames outgoing
// messages with a trailing newline, and reassembles incoming ones the same
// way the server does. Deliberately dumb about game semantics -- callers
// interpret TableMessage::type/payload themselves.
class TableClient : public QObject {
    Q_OBJECT
public:
    explicit TableClient(QObject* parent = nullptr);

    void connectToHost(const QString& host, quint16 port);
    void disconnectFromHost();
    void send(const TableMessage& message);

    [[nodiscard]] bool isConnected() const;

signals:
    void connected();
    void disconnected();
    void messageReceived(const TableMessage& message);
    void connectionError(const QString& errorString);

private:
    void onReadyRead();

    QTcpSocket m_socket;
    QByteArray m_buffer;
};

} // namespace qttutorial::games::common
