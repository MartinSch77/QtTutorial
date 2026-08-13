// SPDX-License-Identifier: MIT
#pragma once

#include "LanAdvertiser.h" // for kLanDiscoveryPort
#include "LanBeacon.h"

#include <QHash>
#include <QTimer>
#include <QUdpSocket>

namespace qttutorial::games::common {

struct DiscoveredGame {
    QString hostAddress; // dotted-quad, taken from the beacon datagram's sender, not the JSON payload
    LanBeacon beacon;
};

// Client side of LAN discovery: listens for LanBeacon broadcasts and keeps
// a live list of games seen in the last few seconds, expiring ones whose
// host has gone quiet (closed or crashed without a graceful goodbye).
class LanBrowser : public QObject {
    Q_OBJECT
public:
    explicit LanBrowser(QObject* parent = nullptr);

    [[nodiscard]] bool start();
    void stop();
    [[nodiscard]] QList<DiscoveredGame> discoveredGames(const QString& gameId = {}) const;

signals:
    void gamesChanged();

private:
    void onReadyRead();
    void expireStaleEntries();

    QUdpSocket m_socket;
    QTimer m_expiryTimer;
    struct Entry {
        LanBeacon beacon;
        qint64 lastSeenMs = 0;
    };
    QHash<QString, Entry> m_entries; // keyed by "hostAddress:tcpPort"
};

} // namespace qttutorial::games::common
