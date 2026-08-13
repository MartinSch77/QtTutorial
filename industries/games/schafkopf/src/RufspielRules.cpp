// SPDX-License-Identifier: MIT
#include "RufspielRules.h"

#include "TrumpOrder.h"

#include <algorithm>

namespace qttutorial::games::schafkopf {

bool isLegalCall(const std::vector<Card>& hand, Suit calledSuit)
{
    if (calledSuit == Suit::Herz) {
        return false;
    }
    const Card calledAce{calledSuit, Rank::Ass};
    if (std::ranges::find(hand, calledAce) != hand.end()) {
        return false;
    }
    return std::ranges::any_of(hand, [calledSuit](const Card& card) {
        return card.suit == calledSuit && !isTrump(card);
    });
}

std::vector<Suit> legalCallOptions(const std::vector<Card>& hand)
{
    std::vector<Suit> options;
    for (Suit suit : allSuits()) {
        if (isLegalCall(hand, suit)) {
            options.push_back(suit);
        }
    }
    return options;
}

bool isLegalPlay(const std::vector<Card>& hand, const std::vector<Card>& trickSoFar,
                  const Card& candidate)
{
    if (trickSoFar.empty()) {
        return true;
    }

    const Card& ledCard = trickSoFar.front();
    if (isTrump(ledCard)) {
        const bool handHasTrump = std::ranges::any_of(hand, [](const Card& c) { return isTrump(c); });
        return !handHasTrump || isTrump(candidate);
    }

    const Suit ledSuit = ledCard.suit;
    const bool handHasLedSuit = std::ranges::any_of(hand, [ledSuit](const Card& c) {
        return !isTrump(c) && c.suit == ledSuit;
    });
    if (!handHasLedSuit) {
        return true;
    }
    return !isTrump(candidate) && candidate.suit == ledSuit;
}

std::vector<Card> legalPlays(const std::vector<Card>& hand, const std::vector<Card>& trickSoFar)
{
    std::vector<Card> result;
    for (const Card& card : hand) {
        if (isLegalPlay(hand, trickSoFar, card)) {
            result.push_back(card);
        }
    }
    return result;
}

} // namespace qttutorial::games::schafkopf
