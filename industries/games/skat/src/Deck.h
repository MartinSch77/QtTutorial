// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <array>
#include <random>
#include <vector>

namespace qttutorial::games::skat {

[[nodiscard]] std::vector<Card> buildDeck();

struct Deal {
    std::array<std::vector<Card>, 3> hands; // 10 cards each, one per active player
    std::vector<Card> skat;                 // 2 face-down cards ("der Skat")
};

// Shuffles a fresh 32-card deck and deals 10 cards to each of the 3 active
// players plus 2 to the Skat. Takes the RNG by reference so tests can pass a
// seeded std::mt19937 for deterministic results.
[[nodiscard]] Deal dealHand(std::mt19937& rng);

} // namespace qttutorial::games::skat
