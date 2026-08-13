// SPDX-License-Identifier: MIT
#pragma once

#include "Bidding.h"
#include "Card.h"
#include "Trump.h"

#include <optional>
#include <random>
#include <vector>

namespace qttutorial::games::skat {

// A basic, deliberately non-strategic stand-in for a missing human player.
// It plays a uniformly random *legal* card (does try to follow suit, since
// otherwise it wouldn't even qualify as a legal Skat player), bids only a
// couple of steps above the baseline before giving up, discards its two
// lowest-value cards, and always declares a Suit game in whichever suit it
// holds the most cards of. It has no notion of hand strength, safe leads,
// or the point count needed to make its bid -- it is a rules-legal
// placeholder, not an opponent.
namespace Bot {

[[nodiscard]] Card chooseCardToPlay(const std::vector<Card>& hand, const std::optional<Card>& ledCard,
                                     GameType gameType, Suit trumpSuit, std::mt19937& rng);

// Raises by exactly 1 while the current highest bid is below a small,
// randomly-picked personal ceiling (baseline + 0..4); passes otherwise.
[[nodiscard]] bool shouldRaise(int highestBid, int baseline, std::mt19937& rng);

[[nodiscard]] std::vector<Card> chooseDiscard(const std::vector<Card>& twelveCards);

struct Announcement {
    GameType gameType = GameType::Suit;
    Suit trumpSuit = Suit::Eichel;
};
[[nodiscard]] Announcement chooseAnnouncement(const std::vector<Card>& tenCards);

} // namespace Bot

} // namespace qttutorial::games::skat
