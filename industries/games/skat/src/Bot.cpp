// SPDX-License-Identifier: MIT
#include "Bot.h"

#include <algorithm>
#include <array>

namespace qttutorial::games::skat::Bot {

Card chooseCardToPlay(const std::vector<Card>& hand, const std::optional<Card>& ledCard, GameType gameType,
                       Suit trumpSuit, std::mt19937& rng)
{
    std::vector<Card> legal;
    if (ledCard) {
        const bool ledIsTrump = isTrump(*ledCard, gameType, trumpSuit);
        for (const Card& card : hand) {
            const bool cardIsTrump = isTrump(card, gameType, trumpSuit);
            const bool followsLed = ledIsTrump ? cardIsTrump : (!cardIsTrump && card.suit == ledCard->suit);
            if (followsLed) {
                legal.push_back(card);
            }
        }
    }
    if (legal.empty()) {
        legal = hand;
    }

    std::uniform_int_distribution<std::size_t> pick(0, legal.size() - 1);
    return legal[pick(rng)];
}

bool shouldRaise(int highestBid, int baseline, std::mt19937& rng)
{
    std::uniform_int_distribution<int> ceilingOffset(0, 4);
    const int personalCeiling = baseline + ceilingOffset(rng);
    return highestBid < personalCeiling;
}

std::vector<Card> chooseDiscard(const std::vector<Card>& twelveCards)
{
    std::vector<Card> sorted = twelveCards;
    std::ranges::sort(sorted, [](const Card& a, const Card& b) { return cardPoints(a.rank) < cardPoints(b.rank); });
    return {sorted[0], sorted[1]};
}

Announcement chooseAnnouncement(const std::vector<Card>& tenCards)
{
    std::array<int, 4> counts{0, 0, 0, 0};
    for (const Card& card : tenCards) {
        ++counts[static_cast<int>(card.suit)];
    }
    const auto maxIt = std::ranges::max_element(counts);
    return Announcement{GameType::Suit, static_cast<Suit>(std::distance(counts.begin(), maxIt))};
}

} // namespace qttutorial::games::skat::Bot
