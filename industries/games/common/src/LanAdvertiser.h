// SPDX-License-Identifier: MIT
#pragma once

#include "LanBeacon.h"

#include <QTimer>
#include <QUdpSocket>

namespace qttutorial::games::common {

// The fixed UDP port every QtTutorial card/kicker game broadcasts its
// discovery beacon on. One port shared across games is fine: LanBeacon's
// gameId field is what lets a browser filter for the game it cares about.
inline constexpr quint16 kLanDiscoveryPort = 45654;

// Host side of LAN discovery: broadcasts a LanBeacon every second so
// LanBrowser instances on the same network segment can find this game
// without the player typing an IP address.
class LanAdvertiser : public QObject {
    Q_OBJECT
public:
    explicit LanAdvertiser(QObject* parent = nullptr);

    void start(LanBeacon beacon);
    void stop();
    void updateSeats(int seatsTaken, int seatsTotal);

private:
    void broadcastOnce();

    QUdpSocket m_socket;
    QTimer m_timer;
    LanBeacon m_beacon;
};

} // namespace qttutorial::games::common
