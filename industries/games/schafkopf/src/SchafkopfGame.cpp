// SPDX-License-Identifier: MIT
#include "SchafkopfGame.h"

#include "RufspielRules.h"
#include "TrumpOrder.h"

#include <QJsonArray>

#include <algorithm>

namespace qttutorial::games::schafkopf {

using qttutorial::games::common::TableMessage;

SchafkopfGame::SchafkopfGame(QObject* parent)
    : QObject(parent)
    , m_rng(std::random_device{}())
{
}

void SchafkopfGame::send(int seat, const QString& type, const QJsonObject& payload)
{
    TableMessage message;
    message.type = type;
    message.seat = seat;
    message.payload = payload;
    emit outgoingMessage(seat, message);
}

void SchafkopfGame::sendError(int seat, const QString& reason)
{
    send(seat, QString::fromLatin1(MessageType::Error), QJsonObject{{QStringLiteral("reason"), reason}});
}

void SchafkopfGame::startNewHand()
{
    dealAndStartBidding();
}

void SchafkopfGame::dealAndStartBidding()
{
    const std::vector<Card> deck = shuffledDeck(m_rng);
    for (int seat = 0; seat < 4; ++seat) {
        m_hands[seat].assign(deck.begin() + seat * 8, deck.begin() + (seat + 1) * 8);
        m_wonCards[seat].clear();
    }

    m_phase = Phase::Bidding;
    m_biddingSeat = (m_dealerSeat + 1) % 4;
    m_firstLeaderSeat = m_biddingSeat;
    m_passesInARow = 0;
    m_callerSeat = -1;
    m_partnerSeat = -1;
    m_calledSuit.reset();
    m_currentTrick.clear();
    m_tricksPlayed = 0;

    for (int seat = 0; seat < 4; ++seat) {
        QJsonArray handIds;
        for (const Card& card : m_hands[seat]) {
            handIds.append(cardToId(card));
        }
        send(seat, QString::fromLatin1(MessageType::HandDealt),
             QJsonObject{{QStringLiteral("dealer"), m_dealerSeat}, {QStringLiteral("hand"), handIds}});
    }
    send(-1, QString::fromLatin1(MessageType::BiddingTurn), QJsonObject{{QStringLiteral("seat"), m_biddingSeat}});
}

void SchafkopfGame::handleTableMessage(int seat, const TableMessage& message)
{
    if (message.type == QLatin1String(MessageType::Call)) {
        processCall(seat, message.payload);
    } else if (message.type == QLatin1String(MessageType::Pass)) {
        processPass(seat);
    } else if (message.type == QLatin1String(MessageType::PlayCard)) {
        processPlayCard(seat, message.payload);
    }
}

void SchafkopfGame::processCall(int seat, const QJsonObject& payload)
{
    if (m_phase != Phase::Bidding || seat != m_biddingSeat) {
        sendError(seat, QStringLiteral("not your turn to bid"));
        return;
    }
    const auto suit = suitFromId(payload.value(QStringLiteral("suit")).toString());
    if (!suit || !isLegalCall(m_hands[seat], *suit)) {
        sendError(seat, QStringLiteral("illegal call"));
        return;
    }

    m_callerSeat = seat;
    m_calledSuit = *suit;
    const Card calledAce{*suit, Rank::Ass};
    for (int other = 0; other < 4; ++other) {
        if (other != seat && std::ranges::find(m_hands[other], calledAce) != m_hands[other].end()) {
            m_partnerSeat = other;
            break;
        }
    }

    send(-1, QString::fromLatin1(MessageType::CallMade),
         QJsonObject{{QStringLiteral("callerSeat"), m_callerSeat}, {QStringLiteral("suit"), suitToId(*suit)}});

    m_phase = Phase::Playing;
    m_turnSeat = m_firstLeaderSeat;
    send(-1, QString::fromLatin1(MessageType::TrickTurn), QJsonObject{{QStringLiteral("seat"), m_turnSeat}});
}

void SchafkopfGame::processPass(int seat)
{
    if (m_phase != Phase::Bidding || seat != m_biddingSeat) {
        sendError(seat, QStringLiteral("not your turn to bid"));
        return;
    }

    m_biddingSeat = (m_biddingSeat + 1) % 4;
    ++m_passesInARow;
    if (m_passesInARow >= 4) {
        m_dealerSeat = (m_dealerSeat + 1) % 4;
        send(-1, QString::fromLatin1(MessageType::Redeal), QJsonObject{});
        dealAndStartBidding();
        return;
    }
    send(-1, QString::fromLatin1(MessageType::BiddingTurn), QJsonObject{{QStringLiteral("seat"), m_biddingSeat}});
}

void SchafkopfGame::processPlayCard(int seat, const QJsonObject& payload)
{
    if (m_phase != Phase::Playing || seat != m_turnSeat) {
        sendError(seat, QStringLiteral("not your turn to play"));
        return;
    }
    const auto card = cardFromId(payload.value(QStringLiteral("cardId")).toString());
    if (!card || std::ranges::find(m_hands[seat], *card) == m_hands[seat].end()) {
        sendError(seat, QStringLiteral("card not in hand"));
        return;
    }

    std::vector<Card> trickCardsSoFar;
    trickCardsSoFar.reserve(m_currentTrick.size());
    for (const auto& [trickSeat, trickCard] : m_currentTrick) {
        trickCardsSoFar.push_back(trickCard);
    }
    if (!isLegalPlay(m_hands[seat], trickCardsSoFar, *card)) {
        sendError(seat, QStringLiteral("illegal play"));
        return;
    }

    std::erase(m_hands[seat], *card);
    m_currentTrick.emplace_back(seat, *card);
    send(-1, QString::fromLatin1(MessageType::CardPlayed),
         QJsonObject{{QStringLiteral("seat"), seat}, {QStringLiteral("cardId"), cardToId(*card)}});

    resolveTrickIfComplete();
}

void SchafkopfGame::resolveTrickIfComplete()
{
    if (m_currentTrick.size() < 4) {
        m_turnSeat = (m_turnSeat + 1) % 4;
        send(-1, QString::fromLatin1(MessageType::TrickTurn), QJsonObject{{QStringLiteral("seat"), m_turnSeat}});
        return;
    }

    std::vector<Card> trickCards;
    trickCards.reserve(4);
    for (const auto& [trickSeat, trickCard] : m_currentTrick) {
        trickCards.push_back(trickCard);
    }
    const std::size_t winnerIndex = trickWinnerIndex(trickCards);
    const int winnerSeat = m_currentTrick[winnerIndex].first;
    int points = 0;
    for (const Card& card : trickCards) {
        points += cardPoints(card);
        m_wonCards[winnerSeat].push_back(card);
    }

    ++m_tricksPlayed;
    m_currentTrick.clear();
    m_turnSeat = winnerSeat;

    send(-1, QString::fromLatin1(MessageType::TrickCompleted),
         QJsonObject{{QStringLiteral("winnerSeat"), winnerSeat}, {QStringLiteral("points"), points}});

    if (m_tricksPlayed >= 8) {
        finishHand();
    } else {
        send(-1, QString::fromLatin1(MessageType::TrickTurn), QJsonObject{{QStringLiteral("seat"), m_turnSeat}});
    }
}

void SchafkopfGame::finishHand()
{
    m_phase = Phase::HandComplete;

    int callerTeamPoints = 0;
    for (const Card& card : m_wonCards[m_callerSeat]) {
        callerTeamPoints += cardPoints(card);
    }
    for (const Card& card : m_wonCards[m_partnerSeat]) {
        callerTeamPoints += cardPoints(card);
    }
    const int opponentTeamPoints = 120 - callerTeamPoints;
    const bool callerTeamWon = callerTeamPoints >= 61;
    const int winningTeamPoints = callerTeamWon ? callerTeamPoints : opponentTeamPoints;
    const int losingTeamPoints = callerTeamWon ? opponentTeamPoints : callerTeamPoints;
    const bool schneider = winningTeamPoints >= 91;
    const bool schwarz = losingTeamPoints == 0;

    send(-1, QString::fromLatin1(MessageType::HandCompleted),
         QJsonObject{
             {QStringLiteral("callerSeat"), m_callerSeat},
             {QStringLiteral("partnerSeat"), m_partnerSeat},
             {QStringLiteral("calledSuit"), suitToId(*m_calledSuit)},
             {QStringLiteral("callerTeamPoints"), callerTeamPoints},
             {QStringLiteral("opponentTeamPoints"), opponentTeamPoints},
             {QStringLiteral("callerTeamWon"), callerTeamWon},
             {QStringLiteral("schneider"), schneider},
             {QStringLiteral("schwarz"), schwarz},
         });
}

} // namespace qttutorial::games::schafkopf
