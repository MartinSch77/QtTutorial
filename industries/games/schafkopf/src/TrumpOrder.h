// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <optional>
#include <vector>

namespace qttutorial::games::schafkopf {

// Rufspiel trump order, highest to lowest: the four Ober (Eichel > Gras >
// Herz > Schellen), then the four Unter (same suit order), then all Herz
// cards by rank (Ass, Zehn, Koenig, Neun, Acht, Sieben). Everything else is
// a plain suit card, ranked Ass > Zehn > Koenig > Neun > Acht > Sieben
// within its own suit.

[[nodiscard]] bool isTrump(const Card& card);

// Higher return value beats lower. std::nullopt for non-trump cards.
[[nodiscard]] std::optional<int> trumpStrength(const Card& card);

// Higher return value beats lower, only comparable within the same suit.
// std::nullopt for trump cards.
[[nodiscard]] std::optional<int> plainStrength(const Card& card);

// Index into cardsInPlayOrder of the card that wins the trick: the highest
// trump if any trump was played, otherwise the highest card of the suit that
// was led (cardsInPlayOrder.front()'s suit, unless it was trump).
[[nodiscard]] std::size_t trickWinnerIndex(const std::vector<Card>& cardsInPlayOrder);

} // namespace qttutorial::games::schafkopf
