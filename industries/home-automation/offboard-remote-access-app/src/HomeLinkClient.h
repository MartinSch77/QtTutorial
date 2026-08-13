// SPDX-License-Identifier: MIT
#pragma once

#include "Protocol.h"

#include <QByteArray>
#include <QObject>
#include <QTcpSocket>

namespace qttutorial::homeautomation::remote {

// A minimal QTcpSocket client speaking the newline-delimited JSON protocol
// from Protocol.h: sends Commands to the home system and emits a signal for
// every Snapshot it receives back. Standing in for "the app on your phone"
// talking to the home over the network.
class HomeLinkClient : public QObject {
    Q_OBJECT
public:
    explicit HomeLinkClient(QObject* parent = nullptr);

    void connectToHome(const QString& host, quint16 port);
    void sendCommand(const Command& command);
    [[nodiscard]] bool isConnected() const;

signals:
    void snapshotReceived(const Snapshot& snapshot);
    void connectionStateChanged(bool connected);

private slots:
    void handleReadyRead();

private:
    QTcpSocket m_socket;
    QByteArray m_buffer;
};

} // namespace qttutorial::homeautomation::remote
