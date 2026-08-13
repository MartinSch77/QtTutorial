// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <vector>

namespace qttutorial::games::schafkopf {

// Legal-call validation: a suit may be called iff it is a plain suit (not
// Herz, which is entirely trump), the caller does not already hold that
// suit's Ass, and the caller holds at least one other, non-trump card of
// that suit ("Ass muss angespielt werden koennen" -- the ace has to be a
// card the caller could plausibly lead into later, which requires having
// more of that suit than just needing its ace to exist in someone else's
// hand).
[[nodiscard]] bool isLegalCall(const std::vector<Card>& hand, Suit calledSuit);

[[nodiscard]] std::vector<Suit> legalCallOptions(const std::vector<Card>& hand);

// Legal-play validation given the cards already played this trick, in play
// order (empty if the hand is leading). Must follow the led suit/trump if
// holding one; otherwise anything in hand is legal.
[[nodiscard]] bool isLegalPlay(const std::vector<Card>& hand, const std::vector<Card>& trickSoFar,
                                const Card& candidate);

[[nodiscard]] std::vector<Card> legalPlays(const std::vector<Card>& hand,
                                            const std::vector<Card>& trickSoFar);

} // namespace qttutorial::games::schafkopf
