// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"
#include "SchafkopfProtocol.h"

#include <QObject>

#include <TableMessage.h>

#include <array>
#include <optional>
#include <random>
#include <vector>

namespace qttutorial::games::schafkopf {

// The authoritative Rufspiel state machine. Runs only on the hosting
// instance: deals, drives bidding, validates and sequences trick-play, and
// scores the hand. It never touches a socket -- it only consumes and
// produces TableMessage values (see outgoingMessage()/handleTableMessage()),
// so the same engine is exercised identically whether a decision came from
// a human via TableClient, or from a local Bot fed straight into
// handleTableMessage() by the app-level bridge. See README.md for the full
// message protocol.
class SchafkopfGame : public QObject {
    Q_OBJECT
public:
    explicit SchafkopfGame(QObject* parent = nullptr);

    // Shuffles, deals 8 cards to each of the 4 seats, and starts bidding.
    // Seat (m_dealerSeat + 1) % 4 gets the first option to call.
    void startNewHand();

public slots:
    void handleTableMessage(int seat, const qttutorial::games::common::TableMessage& message);

signals:
    // seat == -1 means "broadcast to every seat"; otherwise it's a message
    // addressed to exactly that seat (e.g. a private hand, or a rejection).
    void outgoingMessage(int seat, const qttutorial::games::common::TableMessage& message);

private:
    void dealAndStartBidding();
    void processCall(int seat, const QJsonObject& payload);
    void processPass(int seat);
    void processPlayCard(int seat, const QJsonObject& payload);
    void resolveTrickIfComplete();
    void finishHand();
    void send(int seat, const QString& type, const QJsonObject& payload);
    void sendError(int seat, const QString& reason);

    std::mt19937 m_rng;
    Phase m_phase = Phase::Bidding;

    std::array<std::vector<Card>, 4> m_hands;
    std::array<std::vector<Card>, 4> m_wonCards;

    int m_dealerSeat = 3;
    int m_biddingSeat = 0;
    int m_firstLeaderSeat = 0;
    int m_passesInARow = 0;

    int m_callerSeat = -1;
    int m_partnerSeat = -1;
    std::optional<Suit> m_calledSuit;

    std::vector<std::pair<int, Card>> m_currentTrick;
    int m_turnSeat = 0;
    int m_tricksPlayed = 0;
};

} // namespace qttutorial::games::schafkopf
