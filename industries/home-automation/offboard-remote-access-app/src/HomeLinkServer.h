// SPDX-License-Identifier: MIT
#pragma once

#include "HomeSimulator.h"

#include <QByteArray>
#include <QHash>
#include <QTcpServer>
#include <QTimer>

class QTcpSocket;

namespace qttutorial::homeautomation::remote {

// A small QTcpServer standing in for "the home system": applies commands
// from any connected remote client to a HomeSimulator and broadcasts the
// resulting snapshot (and a light simulated sensor blip, independent of any
// client) to every connected client, using the newline-delimited JSON
// protocol from Protocol.h. In this self-contained demo the offboard app
// starts one of these locally and connects its own client to it; in a real
// deployment this would run on the home's own hub, reachable over the
// internet or a local network.
class HomeLinkServer : public QTcpServer {
    Q_OBJECT
public:
    explicit HomeLinkServer(QObject* parent = nullptr);

private slots:
    void handleNewConnection();
    void handleReadyRead();
    void handleClientDisconnected();
    void simulateSensorBlip();

private:
    void broadcastSnapshot();

    HomeSimulator m_simulator;
    QHash<QTcpSocket*, QByteArray> m_buffers;
    QTimer m_sensorTimer;
    int m_tick = 0;
    int m_activeSensor = -1;
};

} // namespace qttutorial::homeautomation::remote
