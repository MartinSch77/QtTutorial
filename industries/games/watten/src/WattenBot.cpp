// SPDX-License-Identifier: MIT
#include "WattenBot.h"

namespace qttutorial::watten {

WattenBot::WattenBot(WattenGame& game, int seat)
    : m_game(game)
    , m_seat(seat)
{
}

void WattenBot::playIfMyTurn()
{
    if (m_game.currentTurnSeat() != m_seat) {
        return;
    }
    const std::vector<Card>& hand = m_game.hand(m_seat);
    if (hand.empty()) {
        return;
    }
    std::uniform_int_distribution<std::size_t> distribution(0, hand.size() - 1);
    const Card chosen = hand[distribution(m_rng)];
    m_game.playCard(m_seat, chosen);
}

} // namespace qttutorial::watten
