// SPDX-License-Identifier: MIT
#include "SkatController.h"

#include <QHostAddress>
#include <QJsonArray>

namespace qttutorial::games::skat {

using common::DiscoveredGame;
using common::LanBeacon;
using common::TableMessage;

SkatController::SkatController(QObject* parent)
    : QObject(parent)
{
    connect(&m_browser, &common::LanBrowser::gamesChanged, this, &SkatController::discoveredGamesChanged);
}

void SkatController::setPhase(const QString& phase)
{
    if (m_phase == phase) {
        return;
    }
    m_phase = phase;
    emit phaseChanged();
}

bool SkatController::isForMe(const TableMessage& message) const
{
    return message.seat == -1 || message.seat == m_localSeat;
}

void SkatController::hostGame(const QString& hostName)
{
    m_isHost = true;
    m_localSeat = 0;
    m_displayName = hostName;

    m_game = std::make_unique<SkatGame>();
    m_game->setSeatHuman(0, true);
    connect(m_game.get(), &SkatGame::send, this, [this](int seat, TableMessage message) {
        if (seat == -1) {
            handleIncomingMessage(message);
            if (m_server) {
                m_server->broadcast(message);
            }
        } else if (seat == m_localSeat) {
            handleIncomingMessage(message);
        } else if (m_server) {
            m_server->sendTo(seat, message);
        }
    });
    connect(m_game.get(), &SkatGame::actionRejected, this, [this](int seat, QString reason) {
        if (seat == m_localSeat) {
            m_statusMessage = reason;
            emit statusMessageChanged();
        } else if (m_server) {
            m_server->sendTo(seat, TableMessage{QStringLiteral("actionRejected"), seat, {{QStringLiteral("reason"), reason}}});
        }
    });

    m_server = std::make_unique<common::TableServer>(SkatGame::kSeatCount);
    connect(m_server.get(), &common::TableServer::seatConnected, this, [this](int seat) {
        m_game->setSeatHuman(seat, true);
        m_server->sendTo(seat, TableMessage{QStringLiteral("seatAssigned"), seat, {{QStringLiteral("seat"), seat}}});
        m_advertiser.updateSeats(m_server->seatCount() + 1, SkatGame::kSeatCount);
    });
    connect(m_server.get(), &common::TableServer::seatDisconnected, this, [this](int seat) {
        m_game->setSeatHuman(seat, false);
        m_advertiser.updateSeats(m_server->seatCount() + 1, SkatGame::kSeatCount);
    });
    connect(m_server.get(), &common::TableServer::messageReceived, this,
            [this](int seat, const TableMessage& message) { m_game->applyMessage(seat, message); });

    if (!m_server->listen()) {
        m_statusMessage = QStringLiteral("Failed to start table server");
        emit statusMessageChanged();
        return;
    }
    m_advertiser.start(LanBeacon{QStringLiteral("skat"), hostName, m_server->serverPort(), 1, SkatGame::kSeatCount});

    m_connected = true;
    emit connectionChanged();
    startNextHand();
}

void SkatController::refreshDiscovery()
{
    if (!m_browser.start()) {
        m_statusMessage = QStringLiteral("Failed to start LAN discovery");
        emit statusMessageChanged();
    }
}

QVariantList SkatController::discoveredGames() const
{
    QVariantList list;
    for (const DiscoveredGame& game : m_browser.discoveredGames(QStringLiteral("skat"))) {
        list.push_back(QVariantMap{{QStringLiteral("hostAddress"), game.hostAddress},
                                    {QStringLiteral("hostName"), game.beacon.hostName},
                                    {QStringLiteral("tcpPort"), game.beacon.tcpPort},
                                    {QStringLiteral("seatsTaken"), game.beacon.seatsTaken},
                                    {QStringLiteral("seatsTotal"), game.beacon.seatsTotal}});
    }
    return list;
}

void SkatController::joinDiscoveredGame(int index, const QString& displayName)
{
    const QList<DiscoveredGame> games = m_browser.discoveredGames(QStringLiteral("skat"));
    if (index < 0 || index >= games.size()) {
        m_statusMessage = QStringLiteral("No such discovered game");
        emit statusMessageChanged();
        return;
    }
    joinManual(games[index].hostAddress, games[index].beacon.tcpPort, displayName);
}

void SkatController::joinManual(const QString& host, int port, const QString& displayName)
{
    m_isHost = false;
    m_displayName = displayName;

    m_client = std::make_unique<common::TableClient>();
    connect(m_client.get(), &common::TableClient::connected, this, [this] {
        m_connected = true;
        emit connectionChanged();
    });
    connect(m_client.get(), &common::TableClient::disconnected, this, [this] {
        m_connected = false;
        emit connectionChanged();
    });
    connect(m_client.get(), &common::TableClient::connectionError, this, [this](const QString& error) {
        m_statusMessage = error;
        emit statusMessageChanged();
    });
    connect(m_client.get(), &common::TableClient::messageReceived, this, &SkatController::handleIncomingMessage);
    m_client->connectToHost(host, static_cast<quint16>(port));
}

void SkatController::sendAction(const QString& type, const QJsonObject& payload)
{
    const TableMessage message{type, m_localSeat, payload};
    if (m_isHost && m_game) {
        m_game->applyMessage(m_localSeat, message);
    } else if (m_client) {
        m_client->send(message);
    }
}

void SkatController::startNextHand()
{
    if (m_isHost && m_game) {
        m_game->beginNewHand();
    }
}

void SkatController::submitBid(int amount)
{
    sendAction(QStringLiteral("bid"), QJsonObject{{QStringLiteral("amount"), amount}});
}

void SkatController::submitPass()
{
    sendAction(QStringLiteral("pass"), QJsonObject{});
}

void SkatController::submitDiscard(const QStringList& cards)
{
    QJsonArray array;
    for (const QString& card : cards) {
        array.push_back(card);
    }
    sendAction(QStringLiteral("discard"), QJsonObject{{QStringLiteral("cards"), array}});
}

void SkatController::submitAnnouncement(const QString& gameType, const QString& trumpSuit)
{
    sendAction(QStringLiteral("announce"),
               QJsonObject{{QStringLiteral("gameType"), gameType}, {QStringLiteral("trumpSuit"), trumpSuit}});
}

void SkatController::playCard(const QString& cardCode)
{
    sendAction(QStringLiteral("playCard"), QJsonObject{{QStringLiteral("card"), cardCode}});
}

void SkatController::handleIncomingMessage(const TableMessage& message)
{
    const QJsonObject& payload = message.payload;

    if (message.type == QStringLiteral("seatAssigned")) {
        handleSeatAssigned(payload);
    } else if (message.type == QStringLiteral("lobby")) {
        handleLobby(payload);
    } else if (message.type == QStringLiteral("hand")) {
        handleHand(message);
    } else if (message.type == QStringLiteral("biddingTurn")) {
        handleBiddingTurn(payload);
    } else if (message.type == QStringLiteral("biddingFinished")) {
        handleBiddingFinished(payload);
    } else if (message.type == QStringLiteral("skat")) {
        handleSkat(message);
    } else if (message.type == QStringLiteral("discardAccepted")) {
        handleDiscardAccepted();
    } else if (message.type == QStringLiteral("announced")) {
        handleAnnounced(payload);
    } else if (message.type == QStringLiteral("cardPlayed")) {
        handleCardPlayed(payload);
    } else if (message.type == QStringLiteral("trickTurn")) {
        handleTrickTurn(payload);
    } else if (message.type == QStringLiteral("trickFinished")) {
        handleTrickFinished(payload);
    } else if (message.type == QStringLiteral("handFinished")) {
        handleHandFinished(payload);
    } else if (message.type == QStringLiteral("actionRejected")) {
        handleActionRejected(message);
    }
}

void SkatController::handleSeatAssigned(const QJsonObject& payload)
{
    m_localSeat = payload.value(QStringLiteral("seat")).toInt();
    emit localSeatChanged();
}

void SkatController::handleLobby(const QJsonObject& payload)
{
    QVariantList activeSeats;
    for (const QJsonValue& value : payload.value(QStringLiteral("activeSeats")).toArray()) {
        activeSeats.push_back(value.toInt());
    }
    QVariantList seatHuman;
    for (const QJsonValue& value : payload.value(QStringLiteral("seatIsHuman")).toArray()) {
        seatHuman.push_back(value.toBool());
    }
    m_tableInfo = QVariantMap{{QStringLiteral("dealerSeat"), payload.value(QStringLiteral("dealerSeat")).toInt()},
                               {QStringLiteral("sittingOutSeat"), payload.value(QStringLiteral("sittingOutSeat")).toInt()},
                               {QStringLiteral("activeSeats"), activeSeats},
                               {QStringLiteral("seatHuman"), seatHuman}};
    emit tableChanged();
}

void SkatController::handleHand(const TableMessage& message)
{
    if (!isForMe(message)) {
        return;
    }
    m_myHand.clear();
    for (const QJsonValue& value : message.payload.value(QStringLiteral("cards")).toArray()) {
        m_myHand.push_back(value.toString());
    }
    emit handChanged();
}

void SkatController::handleBiddingTurn(const QJsonObject& payload)
{
    setPhase(QStringLiteral("bidding"));
    m_biddingInfo = QVariantMap{{QStringLiteral("turnSeat"), payload.value(QStringLiteral("seat")).toInt()},
                                 {QStringLiteral("highestBid"), payload.value(QStringLiteral("highestBid")).toInt()},
                                 {QStringLiteral("highestBidder"), payload.value(QStringLiteral("highestBidder")).toInt()}};
    emit biddingChanged();
}

void SkatController::handleBiddingFinished(const QJsonObject& payload)
{
    setPhase(QStringLiteral("discarding"));
    m_declarationInfo = QVariantMap{{QStringLiteral("declarerSeat"), payload.value(QStringLiteral("declarerSeat")).toInt()},
                                     {QStringLiteral("declarerValue"), payload.value(QStringLiteral("declarerValue")).toInt()}};
    emit declarationChanged();
}

void SkatController::handleSkat(const TableMessage& message)
{
    if (!isForMe(message)) {
        return;
    }
    m_skatForPickup.clear();
    for (const QJsonValue& value : message.payload.value(QStringLiteral("cards")).toArray()) {
        m_skatForPickup.push_back(value.toString());
    }
    emit skatChanged();
}

void SkatController::handleDiscardAccepted()
{
    setPhase(QStringLiteral("announcing"));
    m_skatForPickup.clear();
    emit skatChanged();
}

void SkatController::handleAnnounced(const QJsonObject& payload)
{
    setPhase(QStringLiteral("playing"));
    m_declarationInfo[QStringLiteral("gameType")] = payload.value(QStringLiteral("gameType")).toString();
    m_declarationInfo[QStringLiteral("trumpSuit")] = payload.value(QStringLiteral("trumpSuit")).toString();
    emit declarationChanged();
    m_currentTrick.clear();
    emit trickChanged();
}

void SkatController::handleCardPlayed(const QJsonObject& payload)
{
    m_currentTrick.push_back(
        QVariantMap{{QStringLiteral("seat"), payload.value(QStringLiteral("seat")).toInt()},
                    {QStringLiteral("card"), payload.value(QStringLiteral("card")).toString()}});
    emit trickChanged();
}

void SkatController::handleTrickTurn(const QJsonObject& payload)
{
    m_tableInfo[QStringLiteral("trickTurnSeat")] = payload.value(QStringLiteral("seat")).toInt();
    emit tableChanged();
}

void SkatController::handleTrickFinished(const QJsonObject& payload)
{
    QVariantList cards;
    for (const QJsonValue& value : payload.value(QStringLiteral("cards")).toArray()) {
        const QJsonObject entry = value.toObject();
        cards.push_back(QVariantMap{{QStringLiteral("seat"), entry.value(QStringLiteral("seat")).toInt()},
                                     {QStringLiteral("card"), entry.value(QStringLiteral("card")).toString()}});
    }
    m_lastTrickInfo = QVariantMap{{QStringLiteral("winnerSeat"), payload.value(QStringLiteral("winnerSeat")).toInt()},
                                   {QStringLiteral("points"), payload.value(QStringLiteral("points")).toInt()},
                                   {QStringLiteral("cards"), cards}};
    emit lastTrickChanged();
    m_currentTrick.clear();
    emit trickChanged();
}

void SkatController::handleHandFinished(const QJsonObject& payload)
{
    setPhase(QStringLiteral("handComplete"));
    m_resultInfo = QVariantMap{{QStringLiteral("declarerSeat"), payload.value(QStringLiteral("declarerSeat")).toInt()},
                                {QStringLiteral("declarerPoints"), payload.value(QStringLiteral("declarerPoints")).toInt()},
                                {QStringLiteral("defenderPoints"), payload.value(QStringLiteral("defenderPoints")).toInt()},
                                {QStringLiteral("win"), payload.value(QStringLiteral("win")).toBool()}};
    emit resultChanged();
}

void SkatController::handleActionRejected(const TableMessage& message)
{
    if (!isForMe(message)) {
        return;
    }
    m_statusMessage = message.payload.value(QStringLiteral("reason")).toString();
    emit statusMessageChanged();
}

} // namespace qttutorial::games::skat
