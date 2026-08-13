// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <random>
#include <vector>

namespace qttutorial::maumau {

// The standard German 32-card Skat-pattern deck, French-suited (Clubs/Spades/Hearts/Diamonds)
// for simplicity: this changes no Mau-Mau rule, only the pictures on the cards.
[[nodiscard]] std::vector<Card> fullDeck();

void shuffle(std::vector<Card>& cards, std::mt19937& rng);

} // namespace qttutorial::maumau
