// SPDX-License-Identifier: MIT
#pragma once

#include "TableMessage.h"

#include <QHash>
#include <QObject>
#include <QTcpServer>

class QTcpSocket;

namespace qttutorial::games::common {

// Hosts a LAN table game: accepts up to maxSeats connections, assigns each
// a seat index (0-based, in connection order), and relays newline-delimited
// JSON TableMessages. What "type"/"payload" mean is entirely up to whatever
// game is built on top -- this class only knows about seats and framing,
// the same separation of transport from protocol semantics used in
// framework-tour/04-networking's EchoServer.
class TableServer : public QObject {
    Q_OBJECT
public:
    explicit TableServer(int maxSeats, QObject* parent = nullptr);
    ~TableServer() override;

    [[nodiscard]] bool listen(quint16 port = 0);
    [[nodiscard]] quint16 serverPort() const;
    [[nodiscard]] int seatCount() const;
    [[nodiscard]] int maxSeats() const { return m_maxSeats; }

    void broadcast(const TableMessage& message);
    void sendTo(int seat, const TableMessage& message);

signals:
    void seatConnected(int seat);
    void seatDisconnected(int seat);
    void messageReceived(int seat, const TableMessage& message);

private:
    void onNewConnection();
    void onReadyRead(QTcpSocket* socket, int seat);
    void onDisconnected(int seat);

    QTcpServer m_server;
    int m_maxSeats;
    QHash<int, QTcpSocket*> m_seatSockets;
    QHash<QTcpSocket*, QByteArray> m_buffers;
};

} // namespace qttutorial::games::common
