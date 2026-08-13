// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <optional>

namespace qttutorial::maumau {

// Everything a legality check needs to know about the table: the discard pile's top card, and
// (if the last play was a Jack) the suit its player wished for, which the very next play must
// honor instead of the top card's own suit/rank.
struct DiscardState {
    Card topCard;
    std::optional<Suit> wishedSuit;
};

enum class SpecialEffect { None, DrawTwo, Skip };

// A Jack may always be played, on any top card, regardless of any pending wish -- playing one
// starts a fresh wish. Otherwise a card is legal if it matches the wished suit (when one is
// pending) or, absent a wish, if it matches the top card's suit or rank.
[[nodiscard]] bool isLegalPlay(const Card& card, const DiscardState& state);

[[nodiscard]] SpecialEffect effectOf(Rank rank);

} // namespace qttutorial::maumau
