// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"
#include "SchafkopfProtocol.h"

#include <QObject>

#include <TableMessage.h>

#include <optional>
#include <vector>

namespace qttutorial::games::schafkopf {

// A seat's-eye view of a hand of Rufspiel, built purely by replaying the
// TableMessages a SchafkopfGame produces. Used for every seat -- the
// hosting bridge's own UI seat, its bot seats, and (via TableClient) every
// remote player -- so all of them see identical, network-message-driven
// state with no separate "local" code path. Deliberately Qt-Core-only: it
// never touches a socket.
class SchafkopfClientState : public QObject {
    Q_OBJECT
public:
    struct HandResult {
        int callerSeat = -1;
        int partnerSeat = -1;
        Suit calledSuit = Suit::Eichel;
        int callerTeamPoints = 0;
        int opponentTeamPoints = 0;
        bool callerTeamWon = false;
        bool schneider = false;
        bool schwarz = false;
    };

    explicit SchafkopfClientState(int seat, QObject* parent = nullptr);

    [[nodiscard]] int seat() const { return m_seat; }
    [[nodiscard]] Phase phase() const { return m_phase; }
    [[nodiscard]] const std::vector<Card>& hand() const { return m_hand; }
    [[nodiscard]] int dealerSeat() const { return m_dealerSeat; }
    [[nodiscard]] int biddingSeat() const { return m_biddingSeat; }
    [[nodiscard]] int turnSeat() const { return m_turnSeat; }
    [[nodiscard]] int callerSeat() const { return m_callerSeat; }
    [[nodiscard]] std::optional<Suit> calledSuit() const { return m_calledSuit; }
    [[nodiscard]] const std::vector<std::pair<int, Card>>& currentTrick() const { return m_currentTrick; }
    [[nodiscard]] const std::optional<HandResult>& lastResult() const { return m_lastResult; }

    void applyMessage(const qttutorial::games::common::TableMessage& message);

signals:
    void handDealt();
    void biddingTurnChanged(int seat);
    void callAnnounced(int callerSeat, int suitOrdinal);
    void redealAnnounced();
    void turnChanged(int seat);
    void cardPlayed(int seat, int suitOrdinal, int rankOrdinal);
    void trickCompleted(int winnerSeat, int points);
    void handCompleted();
    void errorReceived(QString reason);

private:
    int m_seat;
    Phase m_phase = Phase::Bidding;
    std::vector<Card> m_hand;
    int m_dealerSeat = -1;
    int m_biddingSeat = -1;
    int m_turnSeat = -1;
    int m_callerSeat = -1;
    std::optional<Suit> m_calledSuit;
    std::vector<std::pair<int, Card>> m_currentTrick;
    std::optional<HandResult> m_lastResult;
};

} // namespace qttutorial::games::schafkopf
