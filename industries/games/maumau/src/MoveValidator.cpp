// SPDX-License-Identifier: MIT
#include "MoveValidator.h"

namespace qttutorial::maumau {

bool isLegalPlay(const Card& card, const DiscardState& state)
{
    if (card.rank == Rank::Jack) {
        return true;
    }
    if (state.wishedSuit.has_value()) {
        return card.suit == *state.wishedSuit;
    }
    return card.suit == state.topCard.suit || card.rank == state.topCard.rank;
}

SpecialEffect effectOf(Rank rank)
{
    switch (rank) {
    case Rank::Seven:
        return SpecialEffect::DrawTwo;
    case Rank::Eight:
        return SpecialEffect::Skip;
    default:
        return SpecialEffect::None;
    }
}

} // namespace qttutorial::maumau
