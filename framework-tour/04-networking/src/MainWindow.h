// SPDX-License-Identifier: MIT
#pragma once

#include "EchoServer.h"

#include <QByteArray>
#include <QElapsedTimer>
#include <QHash>
#include <QMainWindow>

class QLabel;
class QPlainTextEdit;
class QPushButton;
class QTcpSocket;

namespace qttutorial::networking {

// Hosts both ends of the demo: an EchoServer listening on an ephemeral
// loopback port, and a QTcpSocket client that connects to it. Every button
// click sends one JSON request and the round-trip latency is measured
// client-side with a QElapsedTimer around the request/response pair.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void sendRequest();
    void handleSocketReadyRead();

private:
    void appendLog(const QString& line);
    void updateLatencyLabel(qint64 latestLatencyMs);

    EchoServer* m_server;
    QTcpSocket* m_clientSocket;

    QPushButton* m_sendButton;
    QPlainTextEdit* m_log;
    QLabel* m_latencyLabel;

    int m_nextRequestId = 0;
    QHash<int, QElapsedTimer> m_pendingRequests;
    QByteArray m_receiveBuffer;

    qint64 m_totalLatencyMs = 0;
    int m_completedRequestCount = 0;
};

} // namespace qttutorial::networking
