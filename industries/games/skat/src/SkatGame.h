// SPDX-License-Identifier: MIT
#pragma once

#include "Bidding.h"
#include "Card.h"
#include "Deck.h"
#include "Trump.h"

#include <TableMessage.h>

#include <QHash>
#include <QObject>

#include <array>
#include <memory>
#include <optional>
#include <random>
#include <utility>

namespace qttutorial::games::skat {

// The authoritative Skat rules engine: dealing, bidding, Skat pickup and
// discard, game-type announcement, trick sequencing, and win/lose
// determination. Only the hosting process runs an instance of this class
// (mirroring TableServer's role: one authority, several thin clients). It
// is driven entirely by TableMessage in (applyMessage) / out (the send
// signal) and never touches QTcpSocket or QML itself -- see
// industries/games/skat/README.md "TableMessage protocol" for what each type/payload
// means. Seats that are not occupied by a connected human are automatically
// played by the basic Bot from Bot.h, so a hand always has exactly 4 table
// seats (one sitting out as dealer, per authentic 4-player Skat practice)
// regardless of how many humans actually joined.
class SkatGame : public QObject {
    Q_OBJECT
public:
    static constexpr int kSeatCount = 4;

    enum class Phase { Lobby, Bidding, Discarding, Announcing, Playing, HandComplete };

    explicit SkatGame(QObject* parent = nullptr);

    void setSeatHuman(int seat, bool human);
    [[nodiscard]] bool isSeatHuman(int seat) const;

    // Test-only convenience: makes dealing deterministic so tests can drive
    // a full, reproducible hand instead of asserting only on invariants.
    void seedRngForTests(unsigned seed) { m_rng.seed(seed); }

    // Starts (or restarts, after a HandComplete) a new hand: rotates the
    // dealer, deals fresh cards, and enters Bidding. No-op if a hand is
    // already in progress.
    void beginNewHand();

    void applyMessage(int seat, const common::TableMessage& message);

    [[nodiscard]] Phase phase() const { return m_phase; }
    [[nodiscard]] int dealerSeat() const { return m_dealerSeat; }
    [[nodiscard]] int sittingOutSeat() const { return m_dealerSeat; }
    [[nodiscard]] std::array<int, 3> activeSeats() const { return m_activeSeats; }
    [[nodiscard]] const std::vector<Card>& handOf(int seat) const;
    [[nodiscard]] const std::vector<Card>& skat() const { return m_skat; }

    [[nodiscard]] const Bidding* bidding() const { return m_bidding.get(); }
    [[nodiscard]] int declarerSeat() const { return m_declarerSeat; }
    [[nodiscard]] GameType declaredGameType() const { return m_gameType; }
    [[nodiscard]] Suit trumpSuit() const { return m_trumpSuit; }

    [[nodiscard]] const std::vector<std::pair<int, Card>>& currentTrick() const { return m_currentTrick; }
    [[nodiscard]] int declarerPoints() const { return m_declarerPoints; }
    [[nodiscard]] int defenderPoints() const { return m_defenderPoints; }
    [[nodiscard]] bool declarerWon() const { return m_declarerPoints >= 61; }

signals:
    // seat == -1 means "broadcast to every connected seat"; otherwise the
    // message is private to that one seat (e.g. a hand of cards).
    void send(int seat, qttutorial::games::common::TableMessage message);
    void stateChanged();
    void actionRejected(int seat, QString reason);
    void handCompleted(int declarerSeat, int declarerPoints, int defenderPoints, bool declarerWon);

private:
    void applyBidAction(int seat, std::optional<int> bidAmount);
    void runBotBiddingTurns();
    void finishBidding();
    void handleDiscard(int seat, const std::vector<Card>& discarded);
    void handleAnnouncement(int seat, GameType gameType, Suit trumpSuit);
    void applyPlayCardAction(int seat, const Card& card);
    void runBotTrickTurns();
    void finishTrick();
    void finishHand();
    [[nodiscard]] bool isCardLegal(int seat, const Card& card) const;
    [[nodiscard]] int nextActiveSeat(int seat) const;
    [[nodiscard]] int currentTrickTurnSeat() const;
    void sendPrivate(int seat, const QString& type, const QJsonObject& payload);
    void broadcast(const QString& type, const QJsonObject& payload);

    Phase m_phase = Phase::Lobby;
    std::array<bool, kSeatCount> m_seatIsHuman{false, false, false, false};
    int m_dealerSeat = kSeatCount - 1; // rotates to 0 on the first beginNewHand()
    std::array<int, 3> m_activeSeats{0, 1, 2};

    std::mt19937 m_rng{std::random_device{}()};
    QHash<int, std::vector<Card>> m_hands;
    std::vector<Card> m_skat;

    std::unique_ptr<Bidding> m_bidding;
    int m_declarerSeat = -1;
    GameType m_gameType = GameType::Suit;
    Suit m_trumpSuit = Suit::Eichel;

    int m_leaderSeat = -1;
    std::vector<std::pair<int, Card>> m_currentTrick;
    int m_declarerPoints = 0;
    int m_defenderPoints = 0;
    int m_tricksPlayed = 0;
};

} // namespace qttutorial::games::skat
