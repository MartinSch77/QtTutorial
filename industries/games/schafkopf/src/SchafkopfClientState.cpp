// SPDX-License-Identifier: MIT
#include "SchafkopfClientState.h"

#include <QJsonArray>

namespace qttutorial::games::schafkopf {

using qttutorial::games::common::TableMessage;

SchafkopfClientState::SchafkopfClientState(int seat, QObject* parent)
    : QObject(parent)
    , m_seat(seat)
{
}

void SchafkopfClientState::applyMessage(const TableMessage& message)
{
    const QJsonObject& payload = message.payload;

    if (message.type == QLatin1String(MessageType::HandDealt)) {
        m_hand.clear();
        for (const QJsonValue& value : payload.value(QStringLiteral("hand")).toArray()) {
            if (const auto card = cardFromId(value.toString())) {
                m_hand.push_back(*card);
            }
        }
        m_dealerSeat = payload.value(QStringLiteral("dealer")).toInt(-1);
        m_phase = Phase::Bidding;
        m_callerSeat = -1;
        m_calledSuit.reset();
        m_currentTrick.clear();
        m_lastResult.reset();
        emit handDealt();
    } else if (message.type == QLatin1String(MessageType::BiddingTurn)) {
        m_biddingSeat = payload.value(QStringLiteral("seat")).toInt(-1);
        emit biddingTurnChanged(m_biddingSeat);
    } else if (message.type == QLatin1String(MessageType::CallMade)) {
        m_callerSeat = payload.value(QStringLiteral("callerSeat")).toInt(-1);
        m_calledSuit = suitFromId(payload.value(QStringLiteral("suit")).toString());
        m_phase = Phase::Playing;
        if (m_calledSuit) {
            emit callAnnounced(m_callerSeat, static_cast<int>(*m_calledSuit));
        }
    } else if (message.type == QLatin1String(MessageType::Redeal)) {
        emit redealAnnounced();
    } else if (message.type == QLatin1String(MessageType::TrickTurn)) {
        m_turnSeat = payload.value(QStringLiteral("seat")).toInt(-1);
        emit turnChanged(m_turnSeat);
    } else if (message.type == QLatin1String(MessageType::CardPlayed)) {
        const int seat = payload.value(QStringLiteral("seat")).toInt(-1);
        const auto card = cardFromId(payload.value(QStringLiteral("cardId")).toString());
        if (card) {
            if (seat == m_seat) {
                std::erase(m_hand, *card);
            }
            m_currentTrick.emplace_back(seat, *card);
            emit cardPlayed(seat, static_cast<int>(card->suit), static_cast<int>(card->rank));
        }
    } else if (message.type == QLatin1String(MessageType::TrickCompleted)) {
        const int winnerSeat = payload.value(QStringLiteral("winnerSeat")).toInt(-1);
        const int points = payload.value(QStringLiteral("points")).toInt(0);
        m_currentTrick.clear();
        emit trickCompleted(winnerSeat, points);
    } else if (message.type == QLatin1String(MessageType::HandCompleted)) {
        HandResult result;
        result.callerSeat = payload.value(QStringLiteral("callerSeat")).toInt(-1);
        result.partnerSeat = payload.value(QStringLiteral("partnerSeat")).toInt(-1);
        result.calledSuit = suitFromId(payload.value(QStringLiteral("calledSuit")).toString()).value_or(Suit::Eichel);
        result.callerTeamPoints = payload.value(QStringLiteral("callerTeamPoints")).toInt(0);
        result.opponentTeamPoints = payload.value(QStringLiteral("opponentTeamPoints")).toInt(0);
        result.callerTeamWon = payload.value(QStringLiteral("callerTeamWon")).toBool(false);
        result.schneider = payload.value(QStringLiteral("schneider")).toBool(false);
        result.schwarz = payload.value(QStringLiteral("schwarz")).toBool(false);
        m_phase = Phase::HandComplete;
        m_lastResult = result;
        emit handCompleted();
    } else if (message.type == QLatin1String(MessageType::Error)) {
        emit errorReceived(payload.value(QStringLiteral("reason")).toString());
    }
}

} // namespace qttutorial::games::schafkopf
