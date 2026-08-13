// SPDX-License-Identifier: MIT
#include "GameController.h"

#include "MoveValidator.h"

#include <QHostInfo>
#include <QJsonArray>

namespace qttutorial::maumau {

using games::common::TableMessage;

GameController::GameController(QObject* parent)
    : QObject(parent)
{
    if (!m_browser.start()) {
        qWarning("GameController: failed to start LAN discovery browser");
    }
    connect(&m_browser, &games::common::LanBrowser::gamesChanged, this, &GameController::updateDiscoveredGames);

    connect(&m_server, &games::common::TableServer::messageReceived, this,
            [this](int seat, const TableMessage& message) { m_game.handleMessage(seat, message); });
    connect(&m_server, &games::common::TableServer::seatConnected, this, [this](int seat) {
        TableMessage welcome;
        welcome.type = QStringLiteral("welcome");
        welcome.seat = seat;
        welcome.payload[QStringLiteral("seatsTotal")] = kSeatCount;
        m_server.sendTo(seat, welcome);
        m_advertiser.updateSeats(m_server.seatCount(), kSeatCount);
    });
    connect(&m_server, &games::common::TableServer::seatDisconnected, this,
            [this](int) { m_advertiser.updateSeats(m_server.seatCount(), kSeatCount); });

    connect(&m_game, &MauMauGame::seatMessage, this,
            [this](int seat, const TableMessage& message) { m_server.sendTo(seat, message); });
    connect(&m_game, &MauMauGame::broadcastMessage, this,
            [this](const TableMessage& message) { m_server.broadcast(message); });

    connect(&m_client, &games::common::TableClient::messageReceived, this, [this](const TableMessage& message) {
        if (message.type == QStringLiteral("welcome")) {
            m_mySeat = message.seat;
            emit mySeatChanged();
        } else if (message.type == QStringLiteral("hand")) {
            if (message.seat == m_mySeat) {
                applyHand(message);
            }
        } else if (message.type == QStringLiteral("public_state")) {
            applyPublicState(message);
        } else if (message.type == QStringLiteral("notice")) {
            applyNotice(message);
        } else if (message.type == QStringLiteral("action_error")) {
            applyError(message);
        }
    });
    connect(&m_client, &games::common::TableClient::connectionError, this, [this](const QString& text) {
        m_connectionError = text;
        emit connectionErrorChanged();
    });
    connect(&m_client, &games::common::TableClient::connected, this, [this] {
        m_inGame = true;
        emit inGameChanged();
    });
}

void GameController::hostGame(const QString& hostName)
{
    m_isHost = true;
    m_phase = QStringLiteral("lobby");

    if (!m_server.listen(0)) {
        m_connectionError = tr("Could not open a TCP port to host on");
        emit connectionErrorChanged();
        return;
    }

    games::common::LanBeacon beacon;
    beacon.gameId = QStringLiteral("maumau");
    beacon.hostName = hostName.isEmpty() ? QHostInfo::localHostName() : hostName;
    beacon.tcpPort = m_server.serverPort();
    beacon.seatsTaken = 1;
    beacon.seatsTotal = kSeatCount;
    m_advertiser.start(beacon);

    // Connect to ourselves so the host's own seat goes through the identical TableMessage path
    // every other seat uses. This must happen before any remote peer can plausibly join (the
    // beacon above was only just sent, and a human still has to react to it), so the host
    // reliably becomes seat 0.
    m_client.connectToHost(QStringLiteral("127.0.0.1"), m_server.serverPort());

    m_inGame = true;
    emit inGameChanged();
}

void GameController::startRound()
{
    if (!m_isHost) {
        return;
    }
    const int connectedSeats = m_server.seatCount();
    for (int seat = 1; seat < kSeatCount; ++seat) {
        m_game.setBotSeat(seat, seat >= connectedSeats);
    }
    m_advertiser.stop();
    m_game.startNewGame(kSeatCount);
}

void GameController::refreshDiscovery()
{
    updateDiscoveredGames();
}

void GameController::joinDiscovered(int index)
{
    const auto games = m_browser.discoveredGames(QStringLiteral("maumau"));
    if (index < 0 || index >= games.size()) {
        return;
    }
    joinManual(games.at(index).hostAddress, games.at(index).beacon.tcpPort);
}

void GameController::joinManual(const QString& address, int port)
{
    m_isHost = false;
    m_client.connectToHost(address, static_cast<quint16>(port));
}

void GameController::playCard(const QString& rank, const QString& suit, const QString& wish)
{
    TableMessage message;
    message.type = QStringLiteral("play_card");
    message.seat = m_mySeat;
    message.payload[QStringLiteral("rank")] = rank;
    message.payload[QStringLiteral("suit")] = suit;
    if (!wish.isEmpty()) {
        message.payload[QStringLiteral("wish")] = wish;
    }
    m_client.send(message);
}

void GameController::drawCard()
{
    TableMessage message;
    message.type = QStringLiteral("draw_card");
    message.seat = m_mySeat;
    m_client.send(message);
}

bool GameController::isCardLegal(const QString& rank, const QString& suit) const
{
    const auto cardRank = rankFromString(rank.toStdString());
    const auto cardSuit = suitFromString(suit.toStdString());
    const auto topRank = rankFromString(m_topCardRank.toStdString());
    const auto topSuit = suitFromString(m_topCardSuit.toStdString());
    if (!cardRank || !cardSuit || !topRank || !topSuit) {
        return false;
    }

    DiscardState state;
    state.topCard = Card{*topSuit, *topRank};
    if (!m_wishedSuit.isEmpty()) {
        state.wishedSuit = suitFromString(m_wishedSuit.toStdString());
    }
    return isLegalPlay(Card{*cardSuit, *cardRank}, state);
}

void GameController::applyPublicState(const TableMessage& message)
{
    const QJsonObject& payload = message.payload;
    const QJsonObject topCard = payload.value(QStringLiteral("topCard")).toObject();
    m_topCardRank = topCard.value(QStringLiteral("rank")).toString();
    m_topCardSuit = topCard.value(QStringLiteral("suit")).toString();
    m_wishedSuit = payload.value(QStringLiteral("wishedSuit")).toString();
    m_currentSeat = payload.value(QStringLiteral("currentSeat")).toInt();
    m_drawPileCount = payload.value(QStringLiteral("drawPileCount")).toInt();
    m_phase = payload.value(QStringLiteral("phase")).toString();
    m_winnerSeat = (payload.value(QStringLiteral("winnerSeat")).isNull())
        ? -1
        : payload.value(QStringLiteral("winnerSeat")).toInt();

    m_handCounts.clear();
    for (const QJsonValue& value : payload.value(QStringLiteral("handCounts")).toArray()) {
        m_handCounts.append(value.toInt());
    }

    emit publicStateChanged();
}

void GameController::applyHand(const TableMessage& message)
{
    m_myHand.clear();
    for (const QJsonValue& value : message.payload.value(QStringLiteral("cards")).toArray()) {
        const QJsonObject card = value.toObject();
        QVariantMap entry;
        entry[QStringLiteral("rank")] = card.value(QStringLiteral("rank")).toString();
        entry[QStringLiteral("suit")] = card.value(QStringLiteral("suit")).toString();
        m_myHand.append(entry);
    }
    emit myHandChanged();
}

void GameController::applyNotice(const TableMessage& message)
{
    const QString kind = message.payload.value(QStringLiteral("kind")).toString();
    const int seat = message.payload.value(QStringLiteral("seat")).toInt();
    if (kind == QStringLiteral("draw_two")) {
        m_lastNotice = tr("Seat %1 must draw two cards").arg(seat);
    } else if (kind == QStringLiteral("skip")) {
        m_lastNotice = tr("Seat %1's turn was skipped").arg(seat);
    }
    emit lastNoticeChanged();
}

void GameController::applyError(const TableMessage& message)
{
    m_lastNotice = message.payload.value(QStringLiteral("reason")).toString();
    emit lastNoticeChanged();
}

void GameController::updateDiscoveredGames()
{
    m_discoveredGames.clear();
    for (const auto& game : m_browser.discoveredGames(QStringLiteral("maumau"))) {
        QVariantMap entry;
        entry[QStringLiteral("hostName")] = game.beacon.hostName;
        entry[QStringLiteral("hostAddress")] = game.hostAddress;
        entry[QStringLiteral("tcpPort")] = game.beacon.tcpPort;
        entry[QStringLiteral("seatsTaken")] = game.beacon.seatsTaken;
        entry[QStringLiteral("seatsTotal")] = game.beacon.seatsTotal;
        m_discoveredGames.append(entry);
    }
    emit discoveredGamesChanged();
}

} // namespace qttutorial::maumau
