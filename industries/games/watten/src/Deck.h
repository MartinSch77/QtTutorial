// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <random>
#include <vector>

namespace qttutorial::watten {

// The full, unshuffled 32-card Watten deck (4 suits x 8 ranks, no Weli).
[[nodiscard]] std::vector<Card> fullDeck();

// A Fisher-Yates shuffle of fullDeck() using the caller-supplied RNG, kept
// as a free function (rather than std::shuffle at call sites) so tests can
// pin down a seed and know exactly what a "shuffle" produced.
[[nodiscard]] std::vector<Card> shuffledDeck(std::mt19937& rng);

} // namespace qttutorial::watten
