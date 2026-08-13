// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <vector>

namespace qttutorial::watten {

struct PlayedCard {
    int seat = -1;
    Card card;
};

// True if `a` beats `b` when both are trump (same suit as trumpSuit), using
// this implementation's single ranking table (see Card.h). Only meaningful
// when both cards are trumpSuit; callers are expected to have checked that.
[[nodiscard]] bool trumpBeats(const Card& a, const Card& b);

// Resolves one trick: no follow-suit obligation is enforced anywhere in this
// implementation (a real, common Watten trait), so `plays` may mix suits
// freely. The winner is the highest trump played, if any; otherwise the
// highest card of the suit led (plays.front()'s suit) -- a card of neither
// the led suit nor trump can never win a trick. `plays` must be non-empty.
[[nodiscard]] int trickWinnerSeat(const std::vector<PlayedCard>& plays, Suit trumpSuit);

} // namespace qttutorial::watten
