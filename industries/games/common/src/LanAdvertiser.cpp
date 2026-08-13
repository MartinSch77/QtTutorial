// SPDX-License-Identifier: MIT
#include "LanAdvertiser.h"

namespace qttutorial::games::common {

LanAdvertiser::LanAdvertiser(QObject* parent)
    : QObject(parent)
    // Value members must not be given `this` as a QObject parent -- see
    // TableServer.cpp for why that crashes on destruction.
    , m_socket()
    , m_timer()
{
    connect(&m_timer, &QTimer::timeout, this, &LanAdvertiser::broadcastOnce);
}

void LanAdvertiser::start(LanBeacon beacon)
{
    m_beacon = std::move(beacon);
    broadcastOnce();
    m_timer.start(1000);
}

void LanAdvertiser::stop()
{
    m_timer.stop();
}

void LanAdvertiser::updateSeats(int seatsTaken, int seatsTotal)
{
    m_beacon.seatsTaken = seatsTaken;
    m_beacon.seatsTotal = seatsTotal;
}

void LanAdvertiser::broadcastOnce()
{
    m_socket.writeDatagram(m_beacon.encode(), QHostAddress::Broadcast, kLanDiscoveryPort);
}

} // namespace qttutorial::games::common
