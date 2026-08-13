// SPDX-License-Identifier: MIT
#include "WattenGame.h"

#include "Deck.h"

#include <algorithm>

namespace qttutorial::watten {

WattenGame::WattenGame(QObject* parent)
    : QObject(parent)
{
}

void WattenGame::startNewHand()
{
    m_dealerSeat = (m_dealerSeat + 1) % kSeatCount;
    m_turnSeat = m_dealerSeat;
    m_trickIndex = 0;
    m_trumpSuit.reset();
    m_currentTrick.clear();
    m_tricksWonBySeat.fill(0);

    const std::vector<Card> deck = shuffledDeck(m_rng);
    for (int seat = 0; seat < kSeatCount; ++seat) {
        m_hands[static_cast<std::size_t>(seat)].assign(
            deck.begin() + seat * kCardsPerHand, deck.begin() + (seat + 1) * kCardsPerHand);
    }

    emit handStarted(m_dealerSeat);
    emit turnChanged(m_turnSeat);
}

const std::vector<Card>& WattenGame::hand(int seat) const
{
    return m_hands[static_cast<std::size_t>(seat)];
}

int WattenGame::tricksWonBySeat(int seat) const
{
    return m_tricksWonBySeat[static_cast<std::size_t>(seat)];
}

bool WattenGame::canPlay(int seat, const Card& card) const
{
    if (m_bummerlScore.isWon() || seat != m_turnSeat) {
        return false;
    }
    const std::vector<Card>& playerHand = hand(seat);
    // No follow-suit obligation in this implementation: the only
    // requirement is that the card is actually still in the player's hand.
    return std::ranges::find(playerHand, card) != playerHand.end();
}

bool WattenGame::playCard(int seat, const Card& card)
{
    if (!canPlay(seat, card)) {
        return false;
    }

    std::vector<Card>& playerHand = m_hands[static_cast<std::size_t>(seat)];
    playerHand.erase(std::ranges::find(playerHand, card));

    if (!m_trumpSuit.has_value()) {
        m_trumpSuit = card.suit;
        emit trumpDecided(*m_trumpSuit);
    }
    m_currentTrick.push_back(PlayedCard{seat, card});
    emit cardPlayed(seat, card);

    if (static_cast<int>(m_currentTrick.size()) == kSeatCount) {
        resolveTrick();
    } else {
        m_turnSeat = (m_turnSeat + 1) % kSeatCount;
        emit turnChanged(m_turnSeat);
    }
    return true;
}

void WattenGame::resolveTrick()
{
    const int winnerSeat = trickWinnerSeat(m_currentTrick, *m_trumpSuit);
    m_tricksWonBySeat[static_cast<std::size_t>(winnerSeat)] += 1;
    emit trickWon(winnerSeat, m_trickIndex);

    const Team winningTeam = teamForSeat(winnerSeat);
    const int teamTricks = tricksWonBySeat(winnerSeat) + tricksWonBySeat((winnerSeat + 2) % kSeatCount);

    ++m_trickIndex;
    m_currentTrick.clear();

    if (teamTricks >= kTricksToWinHand || m_trickIndex >= kCardsPerHand) {
        finishHand(winningTeam);
        return;
    }

    m_turnSeat = winnerSeat;
    emit turnChanged(m_turnSeat);
}

void WattenGame::finishHand(Team winner)
{
    m_bummerlScore.addHandWin(winner);
    emit handWon(winner);
    if (m_bummerlScore.isWon()) {
        emit bummerlWon(winner);
    }
}

} // namespace qttutorial::watten
