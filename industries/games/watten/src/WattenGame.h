// SPDX-License-Identifier: MIT
#pragma once

#include "BummerlScore.h"
#include "Card.h"
#include "TrumpRanking.h"

#include <QObject>

#include <array>
#include <optional>
#include <random>
#include <vector>

namespace qttutorial::watten {

// Authoritative rules/state machine for one Watten table, independent of
// both the UI and the network transport: the host process owns the one
// instance that matters, and drives it purely from calls translated out of
// TableMessages (see industries/games/watten/README.md for the message protocol built
// on top of this). No Qt6::Network dependency here at all.
//
// One hand: each of the 4 seats is dealt 5 cards from a shuffled 32-card
// deck (20 of 32 cards used; the rest sit out that hand). Whoever leads the
// first trick ("Ausspieler") fixes trump for the whole hand as the suit of
// their led card. There is no obligation to follow suit. A trick's winner
// leads the next trick. The first team to win 3 of the (at most 5) tricks
// takes the hand's single Bummerl point immediately -- once a team reaches
// 3 trick wins the remaining tricks are moot, so the hand ends right there.
class WattenGame : public QObject {
    Q_OBJECT
public:
    static constexpr int kSeatCount = 4;
    static constexpr int kCardsPerHand = 5;
    static constexpr int kTricksToWinHand = 3;

    explicit WattenGame(QObject* parent = nullptr);

    void startNewHand();

    [[nodiscard]] bool canPlay(int seat, const Card& card) const;
    bool playCard(int seat, const Card& card);

    [[nodiscard]] const std::vector<Card>& hand(int seat) const;
    [[nodiscard]] const std::vector<PlayedCard>& currentTrick() const { return m_currentTrick; }
    [[nodiscard]] int currentTurnSeat() const { return m_turnSeat; }
    [[nodiscard]] std::optional<Suit> trumpSuit() const { return m_trumpSuit; }
    [[nodiscard]] int dealerSeat() const { return m_dealerSeat; }
    [[nodiscard]] int tricksWonBySeat(int seat) const;
    [[nodiscard]] const BummerlScore& bummerlScore() const { return m_bummerlScore; }

signals:
    void handStarted(int dealerSeat);
    void trumpDecided(Suit suit);
    void cardPlayed(int seat, Card card);
    void trickWon(int winnerSeat, int trickIndex);
    void handWon(Team team);
    void bummerlWon(Team team);
    void turnChanged(int seat);

private:
    void resolveTrick();
    void finishHand(Team winner);

    std::array<std::vector<Card>, kSeatCount> m_hands;
    std::vector<PlayedCard> m_currentTrick;
    std::optional<Suit> m_trumpSuit;
    int m_dealerSeat = -1;
    int m_turnSeat = 0;
    int m_trickIndex = 0;
    std::array<int, kSeatCount> m_tricksWonBySeat{};
    BummerlScore m_bummerlScore;
    std::mt19937 m_rng{std::random_device{}()};
};

} // namespace qttutorial::watten
