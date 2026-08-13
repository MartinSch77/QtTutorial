// SPDX-License-Identifier: MIT
#include "KickerMatch.h"

#include "KickerProtocol.h"

#include <QHostInfo>

#include <algorithm>

namespace qttutorial::games::kicker {

using common::LanBeacon;
using common::TableMessage;

KickerMatch::KickerMatch(QObject* parent)
    : QObject(parent)
    , m_advertiser()
    , m_tickTimer()
{
    connect(&m_tickTimer, &QTimer::timeout, this, &KickerMatch::onSimulationTick);
}

KickerMatch::~KickerMatch() = default;

bool KickerMatch::hostMatch(int humanSeatCount, quint16 port)
{
    m_humanSeatCount = humanSeatCount;
    m_localSeat = kHostLogicalSeat;

    const int remoteSeats = std::max(0, humanSeatCount - 1);
    m_server = std::make_unique<common::TableServer>(remoteSeats);

    connect(m_server.get(), &common::TableServer::seatConnected, this, [this](int transportSeat) {
        const int logicalSeat = transportSeat + 1;
        m_server->sendTo(transportSeat,
                          TableMessage{QStringLiteral("welcome"), logicalSeat,
                                       QJsonObject{{QStringLiteral("humanSeatCount"), m_humanSeatCount}}});
        m_advertiser.updateSeats(m_server->seatCount() + 1, m_humanSeatCount);
        emit seatConnected(logicalSeat);
    });
    connect(m_server.get(), &common::TableServer::seatDisconnected, this, [this](int transportSeat) {
        m_advertiser.updateSeats(m_server->seatCount() + 1, m_humanSeatCount);
        emit seatDisconnected(transportSeat + 1);
    });
    connect(m_server.get(), &common::TableServer::messageReceived, this, &KickerMatch::onServerMessage);

    if (!m_server->listen(port)) {
        m_server.reset();
        return false;
    }

    m_advertiser.start(LanBeacon{QStringLiteral("kicker"), QHostInfo::localHostName(), m_server->serverPort(), 1,
                                 humanSeatCount});
    m_tickTimer.start(kSimulationTickMs);
    emit seatAssigned(m_localSeat);
    return true;
}

void KickerMatch::joinMatch(const QString& hostAddress, quint16 port)
{
    m_client = std::make_unique<common::TableClient>();
    connect(m_client.get(), &common::TableClient::connected, this, &KickerMatch::connectedToHost);
    connect(m_client.get(), &common::TableClient::disconnected, this, &KickerMatch::disconnectedFromHost);
    connect(m_client.get(), &common::TableClient::connectionError, this, &KickerMatch::connectionError);
    connect(m_client.get(), &common::TableClient::messageReceived, this, &KickerMatch::onClientMessage);
    m_client->connectToHost(hostAddress, port);
}

void KickerMatch::leaveMatch()
{
    m_tickTimer.stop();
    m_advertiser.stop();
    if (m_client) {
        m_client->disconnectFromHost();
        m_client.reset();
    }
    m_server.reset();
    m_localSeat = -1;
    m_humanSeatCount = 0;
    m_state = MatchState{};
    m_inputs = {};
}

void KickerMatch::setLocalInputs(const QList<RodInputEntry>& entries)
{
    if (isHost()) {
        applyInputEntries(m_localSeat, entries);
        return;
    }
    if (m_client) {
        m_client->send(TableMessage{QStringLiteral("input"), m_localSeat, encodeInputPayload(entries)});
    }
}

quint16 KickerMatch::hostedPort() const
{
    return m_server ? m_server->serverPort() : 0;
}

void KickerMatch::applyInputEntries(int /*logicalSeat*/, const QList<RodInputEntry>& entries)
{
    // No seat-ownership validation here: this is a LAN tutorial game, and a
    // client is only ever built to send inputs for the rod(s)
    // KickerSimulation::rodsForSeat() assigned it. See the README for that
    // trust boundary being a deliberate simplification.
    for (const RodInputEntry& entry : entries) {
        if (entry.rod >= 0 && entry.rod < 4) {
            m_inputs[static_cast<std::size_t>(entry.rod)] = entry.input;
        }
    }
}

void KickerMatch::onSimulationTick()
{
    m_state = KickerSimulation::step(m_state, kSimulationTickMs / 1000.0, m_inputs);
    m_server->broadcast(TableMessage{QStringLiteral("state"), -1, encodeMatchState(m_state)});
    emit stateChanged();
}

void KickerMatch::onServerMessage(int transportSeat, const TableMessage& message)
{
    if (message.type == QStringLiteral("input")) {
        applyInputEntries(transportSeat + 1, decodeInputPayload(message.payload));
    }
}

void KickerMatch::onClientMessage(const TableMessage& message)
{
    if (message.type == QStringLiteral("welcome")) {
        m_localSeat = message.seat;
        m_humanSeatCount = message.payload.value(QStringLiteral("humanSeatCount")).toInt();
        emit seatAssigned(m_localSeat);
    } else if (message.type == QStringLiteral("state")) {
        m_state = decodeMatchState(message.payload);
        emit stateChanged();
    }
}

} // namespace qttutorial::games::kicker
