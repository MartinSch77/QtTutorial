// SPDX-License-Identifier: MIT
#pragma once

#include "WattenGame.h"

#include <random>

namespace qttutorial::watten {

// Fills an empty seat with the simplest possible legal opponent: whenever
// it is this seat's turn, it plays a uniformly random card from its own
// hand. It does not evaluate card strength, does not try to help its
// partner, and does not bluff or react to what has been played -- it is a
// placeholder to let a table run with fewer than 4 humans, not an opponent
// with any real skill. Document this honestly rather than calling it "AI".
class WattenBot {
public:
    WattenBot(WattenGame& game, int seat);

    void playIfMyTurn();

private:
    WattenGame& m_game;
    int m_seat;
    std::mt19937 m_rng{std::random_device{}()};
};

} // namespace qttutorial::watten
