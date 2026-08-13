// SPDX-License-Identifier: MIT
#include "WattenTableController.h"

#include <QHostAddress>
#include <QJsonArray>
#include <QVariantMap>

namespace qttutorial::watten {

namespace {

QVariantMap cardToVariant(const Card& card)
{
    QVariantMap map;
    map[QStringLiteral("suit")] = suitName(card.suit);
    map[QStringLiteral("rank")] = rankName(card.rank);
    map[QStringLiteral("label")] = cardLabel(card);
    return map;
}

QString teamLabel(Team team)
{
    return team == Team::TeamA ? QStringLiteral("A") : QStringLiteral("B");
}

} // namespace

WattenTableController::WattenTableController(QObject* parent)
    : QObject(parent)
{
}

WattenTableController::~WattenTableController() = default;

void WattenTableController::hostGame(const QString& hostName)
{
    m_isHost = true;
    m_mySeat = 0;
    m_seatIsBot.fill(true);
    m_seatIsBot[0] = false;

    m_game = std::make_unique<WattenGame>(this);
    setupHostGameSignals();

    m_server = std::make_unique<games::common::TableServer>(WattenGame::kSeatCount, this);
    if (!m_server->listen()) {
        m_statusMessage = QStringLiteral("Failed to start table server");
        emit statusMessageChanged();
        return;
    }

    connect(m_server.get(), &games::common::TableServer::seatConnected, this, [this](int seat) {
        m_seatIsBot[static_cast<std::size_t>(seat)] = false;
        m_server->sendTo(seat, games::common::TableMessage{QStringLiteral("welcome"), seat, {{"seat", seat}}});
        m_advertiser->updateSeats(m_server->seatCount() + 1, WattenGame::kSeatCount);
    });
    connect(m_server.get(), &games::common::TableServer::seatDisconnected, this, [this](int seat) {
        m_seatIsBot[static_cast<std::size_t>(seat)] = true;
        m_bots[static_cast<std::size_t>(seat)] = std::make_unique<WattenBot>(*m_game, seat);
        m_advertiser->updateSeats(m_server->seatCount() + 1, WattenGame::kSeatCount);
    });
    connect(m_server.get(), &games::common::TableServer::messageReceived, this,
            &WattenTableController::onServerMessage);

    m_advertiser = std::make_unique<games::common::LanAdvertiser>(this);
    m_advertiser->start(games::common::LanBeacon{QStringLiteral("watten"), hostName, m_server->serverPort(), 1,
                                                  WattenGame::kSeatCount});

    m_statusMessage = QStringLiteral("Hosting on port %1 - waiting for players").arg(m_server->serverPort());
    emit statusMessageChanged();
    emit roleChanged();
}

void WattenTableController::setupHostGameSignals()
{
    connect(m_game.get(), &WattenGame::handStarted, this, [this](int dealerSeat) {
        applyHandStarted(dealerSeat);
        m_server->broadcast(games::common::TableMessage{QStringLiteral("hand_started"), -1, {{"dealerSeat", dealerSeat}}});
        broadcastHandDealt();
    });
    connect(m_game.get(), &WattenGame::trumpDecided, this, [this](Suit suit) {
        const QString name = suitName(suit);
        applyTrumpDecided(name);
        m_server->broadcast(games::common::TableMessage{QStringLiteral("trump_decided"), -1, {{"suit", name}}});
    });
    connect(m_game.get(), &WattenGame::cardPlayed, this, [this](int seat, Card card) {
        applyCardPlayed(seat, suitName(card.suit), rankName(card.rank));
        m_server->broadcast(games::common::TableMessage{
            QStringLiteral("card_played"), -1, {{"seat", seat}, {"suit", suitName(card.suit)}, {"rank", rankName(card.rank)}}});
    });
    connect(m_game.get(), &WattenGame::turnChanged, this, [this](int seat) {
        applyTurnChanged(seat);
        m_server->broadcast(games::common::TableMessage{QStringLiteral("turn_changed"), -1, {{"seat", seat}}});
        driveBotsIfNeeded();
    });
    connect(m_game.get(), &WattenGame::trickWon, this, [this](int winnerSeat, int trickIndex) {
        m_server->broadcast(games::common::TableMessage{
            QStringLiteral("trick_won"), -1, {{"seat", winnerSeat}, {"trickIndex", trickIndex}}});
    });
    connect(m_game.get(), &WattenGame::handWon, this, [this](Team team) {
        const auto& score = m_game->bummerlScore();
        applyHandWon(teamLabel(team), score.score(Team::TeamA), score.score(Team::TeamB));
        m_server->broadcast(games::common::TableMessage{
            QStringLiteral("hand_won"), -1,
            {{"team", teamLabel(team)}, {"scoreA", score.score(Team::TeamA)}, {"scoreB", score.score(Team::TeamB)}}});
        if (!score.isWon()) {
            m_game->startNewHand();
        }
    });
    connect(m_game.get(), &WattenGame::bummerlWon, this, [this](Team team) {
        applyBummerlWon(teamLabel(team));
        m_server->broadcast(games::common::TableMessage{QStringLiteral("bummerl_won"), -1, {{"team", teamLabel(team)}}});
    });
}

void WattenTableController::broadcastHandDealt()
{
    for (int seat = 0; seat < WattenGame::kSeatCount; ++seat) {
        QJsonArray jsonCards;
        for (const Card& card : m_game->hand(seat)) {
            jsonCards.append(QJsonObject{{"suit", suitName(card.suit)}, {"rank", rankName(card.rank)}});
        }
        if (seat == 0) {
            refreshMyHandFromGame();
        } else if (!m_seatIsBot[static_cast<std::size_t>(seat)]) {
            m_server->sendTo(seat, games::common::TableMessage{QStringLiteral("hand_dealt"), seat, {{"cards", jsonCards}}});
        }
    }
    driveBotsIfNeeded();
}

void WattenTableController::startTable()
{
    if (!m_isHost || !m_game) {
        return;
    }
    for (int seat = 0; seat < WattenGame::kSeatCount; ++seat) {
        if (m_seatIsBot[static_cast<std::size_t>(seat)]) {
            m_bots[static_cast<std::size_t>(seat)] = std::make_unique<WattenBot>(*m_game, seat);
        }
    }
    m_statusMessage = QStringLiteral("Table started");
    emit statusMessageChanged();
    m_game->startNewHand();
}

void WattenTableController::browseLan()
{
    if (!m_browser) {
        m_browser = std::make_unique<games::common::LanBrowser>(this);
        connect(m_browser.get(), &games::common::LanBrowser::gamesChanged, this, [this] {
            m_lanGames.clear();
            for (const auto& discovered : m_browser->discoveredGames(QStringLiteral("watten"))) {
                QVariantMap map;
                map[QStringLiteral("hostName")] = discovered.beacon.hostName;
                map[QStringLiteral("hostAddress")] = discovered.hostAddress;
                map[QStringLiteral("tcpPort")] = discovered.beacon.tcpPort;
                map[QStringLiteral("seatsTaken")] = discovered.beacon.seatsTaken;
                map[QStringLiteral("seatsTotal")] = discovered.beacon.seatsTotal;
                m_lanGames.append(map);
            }
            emit lanGamesChanged();
        });
    }
    if (!m_browser->start()) {
        m_statusMessage = QStringLiteral("Failed to start LAN discovery");
        emit statusMessageChanged();
    }
}

void WattenTableController::joinDiscovered(int index)
{
    if (index < 0 || index >= m_lanGames.size()) {
        return;
    }
    const QVariantMap map = m_lanGames.at(index).toMap();
    joinManual(map.value(QStringLiteral("hostAddress")).toString(), map.value(QStringLiteral("tcpPort")).toInt());
}

void WattenTableController::joinManual(const QString& address, int port)
{
    m_isHost = false;
    m_client = std::make_unique<games::common::TableClient>(this);
    connect(m_client.get(), &games::common::TableClient::connected, this, [this] {
        m_statusMessage = QStringLiteral("Connected - waiting for host to start the table");
        emit statusMessageChanged();
    });
    connect(m_client.get(), &games::common::TableClient::connectionError, this, [this](const QString& error) {
        m_statusMessage = QStringLiteral("Connection failed: %1").arg(error);
        emit statusMessageChanged();
    });
    connect(m_client.get(), &games::common::TableClient::messageReceived, this, &WattenTableController::onClientMessage);

    m_client->connectToHost(address, static_cast<quint16>(port));
    emit roleChanged();
}

void WattenTableController::playCard(const QString& suit, const QString& rank)
{
    const auto suitValue = suitFromName(suit);
    const auto rankValue = rankFromName(rank);
    if (!suitValue || !rankValue) {
        return;
    }
    const Card card{*suitValue, *rankValue};

    if (m_isHost) {
        if (m_game) {
            m_game->playCard(m_mySeat, card);
        }
    } else if (m_client) {
        m_client->send(games::common::TableMessage{QStringLiteral("play_card"), m_mySeat, {{"suit", suit}, {"rank", rank}}});
    }
}

void WattenTableController::applyHandStarted(int dealerSeat)
{
    m_dealerSeat = dealerSeat;
    m_trumpSuit.clear();
    m_trick.clear();
    emit dealerSeatChanged();
    emit trumpSuitChanged();
    emit trickChanged();
}

void WattenTableController::applyTrumpDecided(const QString& suit)
{
    m_trumpSuit = suit;
    emit trumpSuitChanged();
}

void WattenTableController::applyCardPlayed(int seat, const QString& suit, const QString& rank)
{
    if (m_isHost && seat == 0) {
        refreshMyHandFromGame();
    }
    if (m_isHost) {
        refreshTrickFromGame();
    } else {
        QVariantMap map;
        map[QStringLiteral("seat")] = seat;
        map[QStringLiteral("suit")] = suit;
        map[QStringLiteral("rank")] = rank;
        map[QStringLiteral("label")] = rank + QStringLiteral(" ") + suit;
        m_trick.append(map);
    }
    emit trickChanged();
}

void WattenTableController::applyTurnChanged(int seat)
{
    m_turnSeat = seat;
    emit turnSeatChanged();
}

void WattenTableController::applyHandWon(const QString& team, int scoreA, int scoreB)
{
    Q_UNUSED(team);
    m_scoreTeamA = scoreA;
    m_scoreTeamB = scoreB;
    m_trick.clear();
    emit scoresChanged();
    emit trickChanged();
}

void WattenTableController::applyBummerlWon(const QString& team)
{
    m_bummerlWinner = team;
    emit bummerlWinnerChanged();
}

void WattenTableController::refreshMyHandFromGame()
{
    if (!m_game || m_mySeat < 0) {
        return;
    }
    m_myHand.clear();
    for (const Card& card : m_game->hand(m_mySeat)) {
        m_myHand.append(cardToVariant(card));
    }
    emit myHandChanged();
}

void WattenTableController::refreshTrickFromGame()
{
    if (!m_game) {
        return;
    }
    m_trick.clear();
    for (const PlayedCard& played : m_game->currentTrick()) {
        QVariantMap map = cardToVariant(played.card);
        map[QStringLiteral("seat")] = played.seat;
        m_trick.append(map);
    }
}

void WattenTableController::driveBotsIfNeeded()
{
    if (!m_isHost || !m_game) {
        return;
    }
    const int seat = m_game->currentTurnSeat();
    if (m_seatIsBot[static_cast<std::size_t>(seat)] && m_bots[static_cast<std::size_t>(seat)]) {
        m_bots[static_cast<std::size_t>(seat)]->playIfMyTurn();
    }
}

void WattenTableController::onServerMessage(int seat, const games::common::TableMessage& message)
{
    if (message.type != QStringLiteral("play_card") || !m_game) {
        return;
    }
    const QString suit = message.payload.value(QStringLiteral("suit")).toString();
    const QString rank = message.payload.value(QStringLiteral("rank")).toString();
    const auto suitValue = suitFromName(suit);
    const auto rankValue = rankFromName(rank);
    if (!suitValue || !rankValue) {
        return;
    }
    if (!m_game->playCard(seat, Card{*suitValue, *rankValue})) {
        m_server->sendTo(seat, games::common::TableMessage{QStringLiteral("invalid_move"), seat, {{"reason", "not your turn or not in hand"}}});
    }
}

void WattenTableController::onClientMessage(const games::common::TableMessage& message)
{
    if (message.type == QStringLiteral("welcome")) {
        m_mySeat = message.payload.value(QStringLiteral("seat")).toInt();
        emit roleChanged();
    } else if (message.type == QStringLiteral("hand_dealt")) {
        m_myHand.clear();
        for (const QJsonValue& value : message.payload.value(QStringLiteral("cards")).toArray()) {
            const QJsonObject obj = value.toObject();
            m_myHand.append(cardToVariant(Card{*suitFromName(obj.value(QStringLiteral("suit")).toString()),
                                                *rankFromName(obj.value(QStringLiteral("rank")).toString())}));
        }
        emit myHandChanged();
    } else if (message.type == QStringLiteral("hand_started")) {
        applyHandStarted(message.payload.value(QStringLiteral("dealerSeat")).toInt());
    } else if (message.type == QStringLiteral("trump_decided")) {
        applyTrumpDecided(message.payload.value(QStringLiteral("suit")).toString());
    } else if (message.type == QStringLiteral("card_played")) {
        const int playedSeat = message.payload.value(QStringLiteral("seat")).toInt();
        const QString suit = message.payload.value(QStringLiteral("suit")).toString();
        const QString rank = message.payload.value(QStringLiteral("rank")).toString();
        applyCardPlayed(playedSeat, suit, rank);
        if (playedSeat == m_mySeat) {
            for (qsizetype i = 0; i < m_myHand.size(); ++i) {
                const QVariantMap card = m_myHand.at(i).toMap();
                if (card.value(QStringLiteral("suit")).toString() == suit
                    && card.value(QStringLiteral("rank")).toString() == rank) {
                    m_myHand.removeAt(i);
                    break;
                }
            }
            emit myHandChanged();
        }
    } else if (message.type == QStringLiteral("turn_changed")) {
        applyTurnChanged(message.payload.value(QStringLiteral("seat")).toInt());
    } else if (message.type == QStringLiteral("hand_won")) {
        applyHandWon(message.payload.value(QStringLiteral("team")).toString(),
                     message.payload.value(QStringLiteral("scoreA")).toInt(),
                     message.payload.value(QStringLiteral("scoreB")).toInt());
    } else if (message.type == QStringLiteral("bummerl_won")) {
        applyBummerlWon(message.payload.value(QStringLiteral("team")).toString());
    } else if (message.type == QStringLiteral("invalid_move")) {
        m_statusMessage = QStringLiteral("Move rejected: %1").arg(message.payload.value(QStringLiteral("reason")).toString());
        emit statusMessageChanged();
    }
}

} // namespace qttutorial::watten
