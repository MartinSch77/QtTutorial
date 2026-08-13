// SPDX-License-Identifier: MIT
#include "SkatGame.h"

#include "Bot.h"

#include <QJsonArray>

#include <algorithm>

namespace qttutorial::games::skat {

using common::TableMessage;

namespace {

QJsonArray codesOf(const std::vector<Card>& cards)
{
    QJsonArray array;
    for (const Card& card : cards) {
        array.push_back(cardCode(card));
    }
    return array;
}

QString gameTypeToString(GameType gameType)
{
    switch (gameType) {
    case GameType::Suit:
        return QStringLiteral("suit");
    case GameType::Grand:
        return QStringLiteral("grand");
    case GameType::Null:
        return QStringLiteral("null");
    }
    return {};
}

std::optional<GameType> gameTypeFromString(const QString& text)
{
    if (text == QStringLiteral("suit")) {
        return GameType::Suit;
    }
    if (text == QStringLiteral("grand")) {
        return GameType::Grand;
    }
    if (text == QStringLiteral("null")) {
        return GameType::Null;
    }
    return std::nullopt;
}

QString suitToString(Suit suit)
{
    return QString(QChar::fromLatin1("EGHS"[static_cast<int>(suit)]));
}

std::optional<Suit> suitFromString(const QString& text)
{
    if (text.isEmpty()) {
        return std::nullopt;
    }
    switch (text.at(0).toUpper().unicode()) {
    case 'E':
        return Suit::Eichel;
    case 'G':
        return Suit::Gras;
    case 'H':
        return Suit::Herz;
    case 'S':
        return Suit::Schellen;
    default:
        return std::nullopt;
    }
}

} // namespace

SkatGame::SkatGame(QObject* parent)
    : QObject(parent)
{
}

void SkatGame::setSeatHuman(int seat, bool human)
{
    if (seat < 0 || seat >= kSeatCount) {
        return;
    }
    m_seatIsHuman[static_cast<std::size_t>(seat)] = human;
}

bool SkatGame::isSeatHuman(int seat) const
{
    if (seat < 0 || seat >= kSeatCount) {
        return false;
    }
    return m_seatIsHuman[static_cast<std::size_t>(seat)];
}

const std::vector<Card>& SkatGame::handOf(int seat) const
{
    static const std::vector<Card> empty;
    const auto it = m_hands.constFind(seat);
    return it == m_hands.cend() ? empty : it.value();
}

void SkatGame::sendPrivate(int seat, const QString& type, const QJsonObject& payload)
{
    emit send(seat, TableMessage{type, seat, payload});
}

void SkatGame::broadcast(const QString& type, const QJsonObject& payload)
{
    emit send(-1, TableMessage{type, -1, payload});
}

int SkatGame::nextActiveSeat(int seat) const
{
    for (int i = 1; i <= kSeatCount; ++i) {
        const int candidate = (seat + i) % kSeatCount;
        for (int active : m_activeSeats) {
            if (active == candidate) {
                return candidate;
            }
        }
    }
    return -1;
}

void SkatGame::beginNewHand()
{
    if (m_phase != Phase::Lobby && m_phase != Phase::HandComplete) {
        return;
    }

    m_dealerSeat = (m_dealerSeat + 1) % kSeatCount;
    for (int i = 1; i <= 3; ++i) {
        m_activeSeats[static_cast<std::size_t>(i - 1)] = (m_dealerSeat + i) % kSeatCount;
    }

    const Deal deal = dealHand(m_rng);
    m_hands.clear();
    for (std::size_t i = 0; i < m_activeSeats.size(); ++i) {
        m_hands.insert(m_activeSeats[i], deal.hands[i]);
    }
    m_hands.insert(m_dealerSeat, {});
    m_skat = deal.skat;

    m_declarerSeat = -1;
    m_declarerPoints = 0;
    m_defenderPoints = 0;
    m_tricksPlayed = 0;
    m_currentTrick.clear();

    QJsonArray humanFlags;
    QJsonArray activeArray;
    for (int seat : m_activeSeats) {
        activeArray.push_back(seat);
    }
    for (bool human : m_seatIsHuman) {
        humanFlags.push_back(human);
    }
    broadcast(QStringLiteral("lobby"), QJsonObject{
                                            {QStringLiteral("dealerSeat"), m_dealerSeat},
                                            {QStringLiteral("sittingOutSeat"), m_dealerSeat},
                                            {QStringLiteral("activeSeats"), activeArray},
                                            {QStringLiteral("seatIsHuman"), humanFlags},
                                        });

    for (int seat = 0; seat < kSeatCount; ++seat) {
        sendPrivate(seat, QStringLiteral("hand"), QJsonObject{{QStringLiteral("cards"), codesOf(handOf(seat))}});
    }

    m_bidding = std::make_unique<Bidding>(m_activeSeats);
    m_phase = Phase::Bidding;
    broadcast(QStringLiteral("biddingTurn"), QJsonObject{
                                                  {QStringLiteral("seat"), m_bidding->currentTurnSeat()},
                                                  {QStringLiteral("highestBid"), m_bidding->highestBid()},
                                                  {QStringLiteral("highestBidder"), m_bidding->highestBidder()},
                                              });
    emit stateChanged();
    runBotBiddingTurns();
}

void SkatGame::applyBidAction(int seat, std::optional<int> bidAmount)
{
    if (m_phase != Phase::Bidding || !m_bidding) {
        emit actionRejected(seat, QStringLiteral("Bidding is not in progress"));
        return;
    }
    const auto result = bidAmount ? m_bidding->submitBid(seat, *bidAmount) : m_bidding->submitPass(seat);
    if (!result) {
        emit actionRejected(seat, QStringLiteral("Illegal bidding action"));
        return;
    }
    if (bidAmount) {
        broadcast(QStringLiteral("bidAccepted"),
                  QJsonObject{{QStringLiteral("seat"), seat}, {QStringLiteral("amount"), *bidAmount}});
    } else {
        broadcast(QStringLiteral("passAccepted"), QJsonObject{{QStringLiteral("seat"), seat}});
    }
    if (m_bidding->isFinished()) {
        finishBidding();
    } else {
        broadcast(QStringLiteral("biddingTurn"),
                  QJsonObject{{QStringLiteral("seat"), m_bidding->currentTurnSeat()},
                              {QStringLiteral("highestBid"), m_bidding->highestBid()},
                              {QStringLiteral("highestBidder"), m_bidding->highestBidder()}});
    }
    emit stateChanged();
}

void SkatGame::runBotBiddingTurns()
{
    while (m_phase == Phase::Bidding && m_bidding && !isSeatHuman(m_bidding->currentTurnSeat())) {
        const int botSeat = m_bidding->currentTurnSeat();
        if (Bot::shouldRaise(m_bidding->highestBid(), m_bidding->baseline(), m_rng)) {
            const int amount = m_bidding->highestBidder() == -1 ? m_bidding->baseline() : m_bidding->highestBid() + 1;
            applyBidAction(botSeat, amount);
        } else {
            applyBidAction(botSeat, std::nullopt);
        }
    }
}

void SkatGame::finishBidding()
{
    m_declarerSeat = *m_bidding->declarerSeat();
    const int value = m_bidding->declarerValue();
    broadcast(QStringLiteral("biddingFinished"),
              QJsonObject{{QStringLiteral("declarerSeat"), m_declarerSeat}, {QStringLiteral("declarerValue"), value}});

    std::vector<Card>& declarerHand = m_hands[m_declarerSeat];
    declarerHand.insert(declarerHand.end(), m_skat.begin(), m_skat.end());
    sendPrivate(m_declarerSeat, QStringLiteral("skat"), QJsonObject{{QStringLiteral("cards"), codesOf(m_skat)}});

    m_phase = Phase::Discarding;
    emit stateChanged();

    if (!isSeatHuman(m_declarerSeat)) {
        handleDiscard(m_declarerSeat, Bot::chooseDiscard(declarerHand));
    }
}

void SkatGame::handleDiscard(int seat, const std::vector<Card>& discarded)
{
    if (m_phase != Phase::Discarding || seat != m_declarerSeat || discarded.size() != 2 ||
        discarded[0] == discarded[1]) {
        emit actionRejected(seat, QStringLiteral("Illegal discard"));
        return;
    }
    std::vector<Card>& hand = m_hands[seat];
    for (const Card& card : discarded) {
        const auto it = std::find(hand.begin(), hand.end(), card);
        if (it == hand.end()) {
            emit actionRejected(seat, QStringLiteral("Discarded card not in hand"));
            return;
        }
    }
    for (const Card& card : discarded) {
        hand.erase(std::find(hand.begin(), hand.end(), card));
    }
    m_skat = discarded;

    broadcast(QStringLiteral("discardAccepted"), QJsonObject{{QStringLiteral("declarerSeat"), seat}});
    m_phase = Phase::Announcing;
    emit stateChanged();

    if (!isSeatHuman(seat)) {
        const Bot::Announcement announcement = Bot::chooseAnnouncement(hand);
        handleAnnouncement(seat, announcement.gameType, announcement.trumpSuit);
    }
}

void SkatGame::handleAnnouncement(int seat, GameType gameType, Suit trumpSuit)
{
    if (m_phase != Phase::Announcing || seat != m_declarerSeat || gameType == GameType::Null) {
        emit actionRejected(seat, QStringLiteral("Illegal or unimplemented game type (Null is not supported)"));
        return;
    }
    m_gameType = gameType;
    m_trumpSuit = trumpSuit;
    broadcast(QStringLiteral("announced"), QJsonObject{{QStringLiteral("declarerSeat"), seat},
                                                         {QStringLiteral("gameType"), gameTypeToString(gameType)},
                                                         {QStringLiteral("trumpSuit"), suitToString(trumpSuit)}});

    m_phase = Phase::Playing;
    m_leaderSeat = m_activeSeats[0];
    m_currentTrick.clear();
    broadcast(QStringLiteral("trickTurn"), QJsonObject{{QStringLiteral("seat"), m_leaderSeat}});
    emit stateChanged();
    runBotTrickTurns();
}

void SkatGame::runBotTrickTurns()
{
    while (m_phase == Phase::Playing && !isSeatHuman(currentTrickTurnSeat())) {
        const int botSeat = currentTrickTurnSeat();
        const std::optional<Card> ledCard = m_currentTrick.empty()
                                                 ? std::nullopt
                                                 : std::make_optional(m_currentTrick.front().second);
        const Card card = Bot::chooseCardToPlay(handOf(botSeat), ledCard, m_gameType, m_trumpSuit, m_rng);
        applyPlayCardAction(botSeat, card);
    }
}

int SkatGame::currentTrickTurnSeat() const
{
    if (m_phase != Phase::Playing) {
        return -1;
    }
    if (m_currentTrick.empty()) {
        return m_leaderSeat;
    }
    return nextActiveSeat(m_currentTrick.back().first);
}

bool SkatGame::isCardLegal(int seat, const Card& card) const
{
    const std::vector<Card>& hand = handOf(seat);
    if (std::find(hand.begin(), hand.end(), card) == hand.end()) {
        return false;
    }
    if (m_currentTrick.empty()) {
        return true;
    }
    const Card& ledCard = m_currentTrick.front().second;
    const bool ledIsTrump = isTrump(ledCard, m_gameType, m_trumpSuit);
    const bool cardIsTrump = isTrump(card, m_gameType, m_trumpSuit);

    if (ledIsTrump) {
        if (cardIsTrump) {
            return true;
        }
        const bool hasTrump =
            std::any_of(hand.begin(), hand.end(), [&](const Card& c) { return isTrump(c, m_gameType, m_trumpSuit); });
        return !hasTrump;
    }

    if (!cardIsTrump && card.suit == ledCard.suit) {
        return true;
    }
    const bool hasLedSuit = std::any_of(hand.begin(), hand.end(), [&](const Card& c) {
        return !isTrump(c, m_gameType, m_trumpSuit) && c.suit == ledCard.suit;
    });
    return !hasLedSuit;
}

void SkatGame::applyPlayCardAction(int seat, const Card& card)
{
    if (m_phase != Phase::Playing || seat != currentTrickTurnSeat() || !isCardLegal(seat, card)) {
        emit actionRejected(seat, QStringLiteral("Illegal card play"));
        return;
    }
    std::vector<Card>& hand = m_hands[seat];
    hand.erase(std::find(hand.begin(), hand.end(), card));
    m_currentTrick.push_back({seat, card});
    broadcast(QStringLiteral("cardPlayed"),
              QJsonObject{{QStringLiteral("seat"), seat}, {QStringLiteral("card"), cardCode(card)}});

    if (m_currentTrick.size() == 3) {
        finishTrick();
    } else {
        broadcast(QStringLiteral("trickTurn"), QJsonObject{{QStringLiteral("seat"), currentTrickTurnSeat()}});
    }
    emit stateChanged();
}

void SkatGame::finishTrick()
{
    std::vector<Card> playedInOrder;
    playedInOrder.reserve(m_currentTrick.size());
    for (const auto& [trickSeat, trickCard] : m_currentTrick) {
        playedInOrder.push_back(trickCard);
    }
    const std::size_t winnerIndex = trickWinner(playedInOrder, m_gameType, m_trumpSuit);
    const int winnerSeat = m_currentTrick[winnerIndex].first;

    int points = 0;
    QJsonArray cardsArray;
    for (const auto& [trickSeat, trickCard] : m_currentTrick) {
        points += cardPoints(trickCard.rank);
        cardsArray.push_back(QJsonObject{{QStringLiteral("seat"), trickSeat}, {QStringLiteral("card"), cardCode(trickCard)}});
    }
    if (winnerSeat == m_declarerSeat) {
        m_declarerPoints += points;
    } else {
        m_defenderPoints += points;
    }

    broadcast(QStringLiteral("trickFinished"), QJsonObject{{QStringLiteral("winnerSeat"), winnerSeat},
                                                            {QStringLiteral("cards"), cardsArray},
                                                            {QStringLiteral("points"), points}});
    ++m_tricksPlayed;
    m_leaderSeat = winnerSeat;
    m_currentTrick.clear();

    if (m_tricksPlayed == 10) {
        finishHand();
    } else {
        broadcast(QStringLiteral("trickTurn"), QJsonObject{{QStringLiteral("seat"), m_leaderSeat}});
    }
}

void SkatGame::finishHand()
{
    for (const Card& card : m_skat) {
        m_declarerPoints += cardPoints(card.rank);
    }
    m_phase = Phase::HandComplete;
    const bool win = declarerWon();
    broadcast(QStringLiteral("handFinished"), QJsonObject{{QStringLiteral("declarerSeat"), m_declarerSeat},
                                                           {QStringLiteral("declarerPoints"), m_declarerPoints},
                                                           {QStringLiteral("defenderPoints"), m_defenderPoints},
                                                           {QStringLiteral("win"), win},
                                                           {QStringLiteral("skat"), codesOf(m_skat)}});
    emit handCompleted(m_declarerSeat, m_declarerPoints, m_defenderPoints, win);
    emit stateChanged();
}

void SkatGame::applyMessage(int seat, const TableMessage& message)
{
    if (message.type == QStringLiteral("bid")) {
        applyBidAction(seat, message.payload.value(QStringLiteral("amount")).toInt());
        runBotBiddingTurns();
    } else if (message.type == QStringLiteral("pass")) {
        applyBidAction(seat, std::nullopt);
        runBotBiddingTurns();
    } else if (message.type == QStringLiteral("discard")) {
        std::vector<Card> discarded;
        for (const QJsonValue& value : message.payload.value(QStringLiteral("cards")).toArray()) {
            if (const auto card = cardFromCode(value.toString())) {
                discarded.push_back(*card);
            }
        }
        handleDiscard(seat, discarded);
    } else if (message.type == QStringLiteral("announce")) {
        const auto gameType = gameTypeFromString(message.payload.value(QStringLiteral("gameType")).toString());
        const auto trumpSuit = suitFromString(message.payload.value(QStringLiteral("trumpSuit")).toString());
        handleAnnouncement(seat, gameType.value_or(GameType::Null), trumpSuit.value_or(Suit::Eichel));
    } else if (message.type == QStringLiteral("playCard")) {
        if (const auto card = cardFromCode(message.payload.value(QStringLiteral("card")).toString())) {
            applyPlayCardAction(seat, *card);
            runBotTrickTurns();
        } else {
            emit actionRejected(seat, QStringLiteral("Malformed card code"));
        }
    } else {
        emit actionRejected(seat, QStringLiteral("Unknown message type: ") + message.type);
    }
}

} // namespace qttutorial::games::skat
