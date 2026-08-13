// SPDX-License-Identifier: MIT
#include "GameBridge.h"

#include "RufspielRules.h"

namespace qttutorial::games::schafkopf {

using qttutorial::games::common::DiscoveredGame;
using qttutorial::games::common::LanAdvertiser;
using qttutorial::games::common::LanBeacon;
using qttutorial::games::common::LanBrowser;
using qttutorial::games::common::TableClient;
using qttutorial::games::common::TableMessage;
using qttutorial::games::common::TableServer;

namespace {
constexpr auto kGameId = "schafkopf";

std::vector<Card> trickCardsOnly(const std::vector<std::pair<int, Card>>& trick)
{
    std::vector<Card> cards;
    cards.reserve(trick.size());
    for (const auto& [seat, card] : trick) {
        cards.push_back(card);
    }
    return cards;
}
} // namespace

GameBridge::GameBridge(QObject* parent)
    : QObject(parent)
{
}

void GameBridge::setStage(const QString& stage)
{
    if (m_stage == stage) {
        return;
    }
    m_stage = stage;
    emit stageChanged();
}

void GameBridge::setStatusText(const QString& text)
{
    if (m_statusText == text) {
        return;
    }
    m_statusText = text;
    emit statusTextChanged();
}

SchafkopfClientState* GameBridge::myState() const
{
    return m_states.value(m_mySeat, nullptr);
}

Bot* GameBridge::botFor(int seat)
{
    if (auto* existing = m_bots.value(seat, nullptr)) {
        return existing;
    }
    auto* bot = new Bot();
    m_bots.insert(seat, bot);
    return bot;
}

void GameBridge::hostGame(const QString& playerName)
{
    m_isHost = true;
    m_mySeat = 0;
    emit mySeatChanged();

    m_server = new TableServer(3, this);
    if (!m_server->listen(0)) {
        setStatusText(QStringLiteral("Could not open a TCP port to host on."));
        return;
    }

    for (int seat = 0; seat < 4; ++seat) {
        m_states.insert(seat, new SchafkopfClientState(seat, this));
    }

    m_game = new SchafkopfGame(this);
    wireGame();

    connect(m_server, &qttutorial::games::common::TableServer::seatConnected, this, [this](int serverSeat) {
        const int logicalSeat = serverSeat + 1;
        m_connectedServerSeats.insert(logicalSeat);
        m_bots.remove(logicalSeat);
        TableMessage welcome;
        welcome.type = QString::fromLatin1(MessageType::Welcome);
        welcome.payload = QJsonObject{{QStringLiteral("seat"), logicalSeat}};
        m_server->sendTo(serverSeat, welcome);
        if (m_advertiser) {
            m_advertiser->updateSeats(1 + static_cast<int>(m_connectedServerSeats.size()), 4);
        }
        setStatusText(QStringLiteral("Player joined seat %1.").arg(logicalSeat));
    });
    connect(m_server, &qttutorial::games::common::TableServer::seatDisconnected, this, [this](int serverSeat) {
        const int logicalSeat = serverSeat + 1;
        m_connectedServerSeats.remove(logicalSeat);
        if (m_advertiser) {
            m_advertiser->updateSeats(1 + static_cast<int>(m_connectedServerSeats.size()), 4);
        }
        setStatusText(QStringLiteral("Seat %1 disconnected; a bot will take over next hand.").arg(logicalSeat));
    });
    connect(m_server, &qttutorial::games::common::TableServer::messageReceived, this,
            [this](int serverSeat, const TableMessage& message) {
                m_game->handleTableMessage(serverSeat + 1, message);
            });

    m_advertiser = new LanAdvertiser(this);
    LanBeacon beacon;
    beacon.gameId = QString::fromLatin1(kGameId);
    beacon.hostName = playerName.isEmpty() ? QStringLiteral("Schafkopf table") : playerName;
    beacon.tcpPort = m_server->serverPort();
    beacon.seatsTaken = 1;
    beacon.seatsTotal = 4;
    m_advertiser->start(beacon);

    setStage(QStringLiteral("lobby"));
    setStatusText(QStringLiteral("Hosting on port %1. Waiting for players (bots fill empty seats).")
                      .arg(static_cast<int>(m_server->serverPort())));
}

void GameBridge::wireGame()
{
    connect(m_game, &SchafkopfGame::outgoingMessage, this, [this](int seat, const TableMessage& message) {
        if (seat == -1) {
            m_server->broadcast(message);
            for (int localSeat = 0; localSeat < 4; ++localSeat) {
                if (!m_connectedServerSeats.contains(localSeat)) {
                    deliverLocal(localSeat, message);
                }
            }
        } else if (m_connectedServerSeats.contains(seat)) {
            m_server->sendTo(seat - 1, message);
        } else {
            deliverLocal(seat, message);
        }
    });
}

void GameBridge::deliverLocal(int seat, const TableMessage& message)
{
    auto* state = m_states.value(seat, nullptr);
    if (!state) {
        return;
    }
    state->applyMessage(message);
    if (seat == m_mySeat) {
        emit stateChanged();
    } else {
        maybeActBot(seat);
    }
}

void GameBridge::maybeActBot(int seat)
{
    auto* state = m_states.value(seat, nullptr);
    if (!state) {
        return;
    }

    if (state->phase() == Phase::Bidding && state->biddingSeat() == seat) {
        Bot* bot = botFor(seat);
        TableMessage message;
        if (const auto suit = bot->chooseCall(state->hand())) {
            message.type = QString::fromLatin1(MessageType::Call);
            message.payload = QJsonObject{{QStringLiteral("suit"), suitToId(*suit)}};
        } else {
            message.type = QString::fromLatin1(MessageType::Pass);
        }
        message.seat = seat;
        m_game->handleTableMessage(seat, message);
    } else if (state->phase() == Phase::Playing && state->turnSeat() == seat) {
        Bot* bot = botFor(seat);
        const Card card = bot->choosePlay(state->hand(), trickCardsOnly(state->currentTrick()));
        TableMessage message;
        message.type = QString::fromLatin1(MessageType::PlayCard);
        message.seat = seat;
        message.payload = QJsonObject{{QStringLiteral("cardId"), cardToId(card)}};
        m_game->handleTableMessage(seat, message);
    }
}

void GameBridge::startPlaying()
{
    if (!m_isHost || !m_game) {
        return;
    }
    for (int seat = 1; seat < 4; ++seat) {
        if (!m_connectedServerSeats.contains(seat)) {
            static_cast<void>(botFor(seat));
        }
    }
    setStage(QStringLiteral("playing"));
    setStatusText(QStringLiteral("Dealing..."));
    m_game->startNewHand();
}

void GameBridge::nextHand()
{
    if (m_isHost && m_game) {
        m_game->startNewHand();
    }
}

void GameBridge::startDiscovery()
{
    if (!m_browser) {
        m_browser = new LanBrowser(this);
        connect(m_browser, &LanBrowser::gamesChanged, this, &GameBridge::discoveredGamesChanged);
        static_cast<void>(m_browser->start());
    }
}

void GameBridge::joinGame(const QString& hostAddress, int port, const QString& playerName)
{
    m_isHost = false;
    Q_UNUSED(playerName);
    m_client = new TableClient(this);
    connect(m_client, &qttutorial::games::common::TableClient::connected, this, [this] {
        setStage(QStringLiteral("lobby"));
        setStatusText(QStringLiteral("Connected. Waiting for the host to deal."));
    });
    connect(m_client, &qttutorial::games::common::TableClient::connectionError, this, [this](const QString& error) {
        setStatusText(QStringLiteral("Connection failed: %1").arg(error));
    });
    connect(m_client, &qttutorial::games::common::TableClient::messageReceived, this,
            [this](const TableMessage& message) {
                if (message.type == QLatin1String(MessageType::Welcome)) {
                    m_mySeat = message.payload.value(QStringLiteral("seat")).toInt(-1);
                    m_states.insert(m_mySeat, new SchafkopfClientState(m_mySeat, this));
                    emit mySeatChanged();
                    setStatusText(QStringLiteral("Seated at seat %1.").arg(m_mySeat));
                    return;
                }
                if (auto* state = myState()) {
                    state->applyMessage(message);
                    if (message.type == QLatin1String(MessageType::HandDealt)) {
                        setStage(QStringLiteral("playing"));
                    }
                    emit stateChanged();
                }
            });
    m_client->connectToHost(hostAddress, static_cast<quint16>(port));
    setStatusText(QStringLiteral("Connecting to %1:%2...").arg(hostAddress).arg(port));
}

void GameBridge::joinDiscovered(int index, const QString& playerName)
{
    if (!m_browser) {
        return;
    }
    const QList<DiscoveredGame> games = m_browser->discoveredGames(QString::fromLatin1(kGameId));
    if (index < 0 || index >= games.size()) {
        return;
    }
    joinGame(games.at(index).hostAddress, games.at(index).beacon.tcpPort, playerName);
}

void GameBridge::callSuit(const QString& suitId)
{
    TableMessage message;
    message.type = QString::fromLatin1(MessageType::Call);
    message.seat = m_mySeat;
    message.payload = QJsonObject{{QStringLiteral("suit"), suitId}};
    if (m_isHost) {
        m_game->handleTableMessage(m_mySeat, message);
    } else if (m_client) {
        m_client->send(message);
    }
}

void GameBridge::pass()
{
    TableMessage message;
    message.type = QString::fromLatin1(MessageType::Pass);
    message.seat = m_mySeat;
    if (m_isHost) {
        m_game->handleTableMessage(m_mySeat, message);
    } else if (m_client) {
        m_client->send(message);
    }
}

void GameBridge::playCard(const QString& cardId)
{
    TableMessage message;
    message.type = QString::fromLatin1(MessageType::PlayCard);
    message.seat = m_mySeat;
    message.payload = QJsonObject{{QStringLiteral("cardId"), cardId}};
    if (m_isHost) {
        m_game->handleTableMessage(m_mySeat, message);
    } else if (m_client) {
        m_client->send(message);
    }
}

QString GameBridge::handPhase() const
{
    auto* state = myState();
    if (!state) {
        return QStringLiteral("none");
    }
    switch (state->phase()) {
    case Phase::Bidding:
        return QStringLiteral("bidding");
    case Phase::Playing:
        return QStringLiteral("playing");
    case Phase::HandComplete:
        return QStringLiteral("handComplete");
    }
    return QStringLiteral("none");
}

int GameBridge::hostPort() const
{
    return m_server ? m_server->serverPort() : 0;
}

int GameBridge::dealerSeat() const
{
    auto* state = myState();
    return state ? state->dealerSeat() : -1;
}

int GameBridge::biddingSeat() const
{
    auto* state = myState();
    return state ? state->biddingSeat() : -1;
}

int GameBridge::turnSeat() const
{
    auto* state = myState();
    return state ? state->turnSeat() : -1;
}

int GameBridge::callerSeat() const
{
    auto* state = myState();
    return state ? state->callerSeat() : -1;
}

QString GameBridge::calledSuitId() const
{
    auto* state = myState();
    if (!state || !state->calledSuit()) {
        return {};
    }
    return suitToId(*state->calledSuit());
}

bool GameBridge::isMyTurnToBid() const
{
    auto* state = myState();
    return state && state->phase() == Phase::Bidding && state->biddingSeat() == m_mySeat;
}

bool GameBridge::isMyTurnToPlay() const
{
    auto* state = myState();
    return state && state->phase() == Phase::Playing && state->turnSeat() == m_mySeat;
}

QVariantList GameBridge::myHand() const
{
    QVariantList result;
    auto* state = myState();
    if (!state) {
        return result;
    }
    const bool bidding = state->phase() == Phase::Bidding;
    const std::vector<Card> trickCards = trickCardsOnly(state->currentTrick());
    for (const Card& card : state->hand()) {
        QVariantMap entry;
        entry[QStringLiteral("id")] = cardToId(card);
        entry[QStringLiteral("suit")] = QString::fromUtf8(suitName(card.suit).data());
        entry[QStringLiteral("rank")] = QString::fromUtf8(rankName(card.rank).data());
        entry[QStringLiteral("points")] = cardPoints(card);
        const bool legal = bidding ? false : isLegalPlay(state->hand(), trickCards, card);
        entry[QStringLiteral("legal")] = legal;
        result.append(entry);
    }
    return result;
}

QVariantList GameBridge::legalCallSuits() const
{
    QVariantList result;
    auto* state = myState();
    if (!state) {
        return result;
    }
    for (Suit suit : legalCallOptions(state->hand())) {
        result.append(suitToId(suit));
    }
    return result;
}

QVariantList GameBridge::trick() const
{
    QVariantList result;
    auto* state = myState();
    if (!state) {
        return result;
    }
    for (const auto& [seat, card] : state->currentTrick()) {
        QVariantMap entry;
        entry[QStringLiteral("seat")] = seat;
        entry[QStringLiteral("id")] = cardToId(card);
        result.append(entry);
    }
    return result;
}

QVariantMap GameBridge::lastResult() const
{
    QVariantMap result;
    auto* state = myState();
    if (!state || !state->lastResult()) {
        return result;
    }
    const auto& r = *state->lastResult();
    result[QStringLiteral("callerSeat")] = r.callerSeat;
    result[QStringLiteral("partnerSeat")] = r.partnerSeat;
    result[QStringLiteral("calledSuit")] = suitToId(r.calledSuit);
    result[QStringLiteral("callerTeamPoints")] = r.callerTeamPoints;
    result[QStringLiteral("opponentTeamPoints")] = r.opponentTeamPoints;
    result[QStringLiteral("callerTeamWon")] = r.callerTeamWon;
    result[QStringLiteral("schneider")] = r.schneider;
    result[QStringLiteral("schwarz")] = r.schwarz;
    return result;
}

QVariantList GameBridge::discoveredGames() const
{
    QVariantList result;
    if (!m_browser) {
        return result;
    }
    for (const DiscoveredGame& game : m_browser->discoveredGames(QString::fromLatin1(kGameId))) {
        QVariantMap entry;
        entry[QStringLiteral("hostAddress")] = game.hostAddress;
        entry[QStringLiteral("hostName")] = game.beacon.hostName;
        entry[QStringLiteral("port")] = game.beacon.tcpPort;
        entry[QStringLiteral("seatsTaken")] = game.beacon.seatsTaken;
        entry[QStringLiteral("seatsTotal")] = game.beacon.seatsTotal;
        result.append(entry);
    }
    return result;
}

} // namespace qttutorial::games::schafkopf
