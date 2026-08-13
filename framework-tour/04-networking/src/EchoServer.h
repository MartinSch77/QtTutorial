// SPDX-License-Identifier: MIT
#pragma once

#include <QByteArray>
#include <QHash>
#include <QTcpServer>

class QTcpSocket;

namespace qttutorial::networking {

// A minimal QTcpServer that speaks the newline-delimited JSON protocol from
// Protocol.h: for every request line it receives, it replies with a response
// line carrying the same id, an echo payload, and its own timestamp.
class EchoServer : public QTcpServer {
    Q_OBJECT
public:
    explicit EchoServer(QObject* parent = nullptr);

private slots:
    void handleNewConnection();
    void handleReadyRead();
    void handleClientDisconnected();

private:
    // Per-connection line buffer: a single readyRead() can deliver a partial
    // or multiple concatenated JSON lines, so bytes accumulate here until a
    // '\n' terminator is found.
    QHash<QTcpSocket*, QByteArray> m_buffers;
};

} // namespace qttutorial::networking
