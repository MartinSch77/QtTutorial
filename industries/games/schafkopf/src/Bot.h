// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <optional>
#include <random>
#include <vector>

namespace qttutorial::games::schafkopf {

// Fills empty seats so the game is playable solo or with fewer than 4
// humans. Deliberately unsophisticated: it plays a legal random card and,
// when bidding, calls the first legal ace it finds rather than evaluating
// hand strength. It does not implement suit-signaling, trump-counting, or
// any other real Schafkopf strategy -- treat it as "a warm body to fill a
// seat", not a challenging opponent.
class Bot {
public:
    explicit Bot(std::mt19937::result_type seed = std::random_device{}());

    // std::nullopt means "pass".
    [[nodiscard]] std::optional<Suit> chooseCall(const std::vector<Card>& hand);

    [[nodiscard]] Card choosePlay(const std::vector<Card>& hand, const std::vector<Card>& trickSoFar);

private:
    std::mt19937 m_rng;
};

} // namespace qttutorial::games::schafkopf
