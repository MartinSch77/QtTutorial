// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <vector>

namespace qttutorial::games::skat {

// A Null game is optional per the project brief and not yet implemented (see
// README "Simplifications" -- its win condition is "zero tricks", entirely
// different from the point-count threshold Suit/Grand use, and would need
// its own tested code path). The enumerator exists so callers/protocol code
// can already name it; SkatGame rejects it for now.
enum class GameType { Suit, Grand, Null };

// True for the 4 Unters in every game type except Null (which has no trump
// at all). In a Suit game, also true for the 4 Obers and for the remaining
// cards of trumpSuit. In a Grand game, only the Unters are trump -- every
// other card, including Obers, belongs to its own plain suit. trumpSuit is
// ignored for Grand and Null.
[[nodiscard]] bool isTrump(const Card& card, GameType gameType, Suit trumpSuit);

// Strength of a card for comparing it against other cards of the same
// category (both trump, or both members of the same led plain suit).
// Comparing strengths of cards from *different* categories is meaningless;
// use trickWinner for that.
//
// Trump order (highest first), per this tutorial's simplified rule set --
// see README "Simplifications" for how this differs from the official
// German-scoring trump ladder:
//   1. The 4 Unters: Eichel > Gras > Herz > Schellen
//   2. (Suit game only) The 4 Obers, same suit order
//   3. (Suit game only) trumpSuit's own remaining cards: Ass > 10 > König > 9 > 8 > 7
// Plain-suit order (highest first): Ass > 10 > König > [Ober, Grand only] > 9 > 8 > 7
[[nodiscard]] int cardStrength(const Card& card, GameType gameType, Suit trumpSuit);

// Determines the winner of one complete trick. playedInOrder[0] is the card
// led; trump beats every plain card regardless of what was led, matching
// standard trick-taking rules -- if no trump was played, the highest card of
// the suit led wins.
[[nodiscard]] std::size_t trickWinner(const std::vector<Card>& playedInOrder, GameType gameType,
                                       Suit trumpSuit);

} // namespace qttutorial::games::skat
