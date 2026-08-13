// SPDX-License-Identifier: MIT
#include "TrumpRanking.h"

#include <algorithm>

namespace qttutorial::watten {

bool trumpBeats(const Card& a, const Card& b)
{
    return rankValue(a.rank) > rankValue(b.rank);
}

int trickWinnerSeat(const std::vector<PlayedCard>& plays, Suit trumpSuit)
{
    Q_ASSERT(!plays.empty());

    const Suit ledSuit = plays.front().card.suit;
    const bool anyTrump = std::any_of(plays.begin(), plays.end(),
                                       [&](const PlayedCard& play) { return play.card.suit == trumpSuit; });
    const Suit decidingSuit = anyTrump ? trumpSuit : ledSuit;

    const PlayedCard* best = nullptr;
    for (const PlayedCard& play : plays) {
        if (play.card.suit != decidingSuit) {
            continue;
        }
        if (best == nullptr || rankValue(play.card.rank) > rankValue(best->card.rank)) {
            best = &play;
        }
    }
    return best->seat;
}

} // namespace qttutorial::watten
