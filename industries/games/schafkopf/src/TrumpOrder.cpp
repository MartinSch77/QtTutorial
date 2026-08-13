// SPDX-License-Identifier: MIT
#include "TrumpOrder.h"

#include <algorithm>
#include <array>
#include <cassert>

namespace qttutorial::games::schafkopf {

namespace {

// Highest to lowest; index 0 is the strongest trump in the game (Eichel-Ober).
constexpr std::array<Card, 14> kTrumpOrderHighToLow{
    Card{Suit::Eichel, Rank::Ober},   Card{Suit::Gras, Rank::Ober},
    Card{Suit::Herz, Rank::Ober},     Card{Suit::Schellen, Rank::Ober},
    Card{Suit::Eichel, Rank::Unter},  Card{Suit::Gras, Rank::Unter},
    Card{Suit::Herz, Rank::Unter},    Card{Suit::Schellen, Rank::Unter},
    Card{Suit::Herz, Rank::Ass},      Card{Suit::Herz, Rank::Zehn},
    Card{Suit::Herz, Rank::Koenig},   Card{Suit::Herz, Rank::Neun},
    Card{Suit::Herz, Rank::Acht},     Card{Suit::Herz, Rank::Sieben},
};

// Highest to lowest, within a single plain suit.
constexpr std::array<Rank, 6> kPlainOrderHighToLow{
    Rank::Ass, Rank::Zehn, Rank::Koenig, Rank::Neun, Rank::Acht, Rank::Sieben,
};

} // namespace

bool isTrump(const Card& card)
{
    return std::ranges::find(kTrumpOrderHighToLow, card) != kTrumpOrderHighToLow.end();
}

std::optional<int> trumpStrength(const Card& card)
{
    const auto it = std::ranges::find(kTrumpOrderHighToLow, card);
    if (it == kTrumpOrderHighToLow.end()) {
        return std::nullopt;
    }
    const auto index = std::distance(kTrumpOrderHighToLow.begin(), it);
    return static_cast<int>(kTrumpOrderHighToLow.size()) - static_cast<int>(index);
}

std::optional<int> plainStrength(const Card& card)
{
    if (isTrump(card)) {
        return std::nullopt;
    }
    const auto it = std::ranges::find(kPlainOrderHighToLow, card.rank);
    assert(it != kPlainOrderHighToLow.end());
    const auto index = std::distance(kPlainOrderHighToLow.begin(), it);
    return static_cast<int>(kPlainOrderHighToLow.size()) - static_cast<int>(index);
}

std::size_t trickWinnerIndex(const std::vector<Card>& cardsInPlayOrder)
{
    assert(!cardsInPlayOrder.empty());

    std::size_t bestIndex = 0;
    int bestTrumpStrength = -1;
    bool anyTrump = false;
    for (std::size_t i = 0; i < cardsInPlayOrder.size(); ++i) {
        if (const auto strength = trumpStrength(cardsInPlayOrder[i])) {
            anyTrump = true;
            if (*strength > bestTrumpStrength) {
                bestTrumpStrength = *strength;
                bestIndex = i;
            }
        }
    }
    if (anyTrump) {
        return bestIndex;
    }

    const Suit ledSuit = cardsInPlayOrder.front().suit;
    int bestPlainStrength = -1;
    bestIndex = 0;
    for (std::size_t i = 0; i < cardsInPlayOrder.size(); ++i) {
        const Card& card = cardsInPlayOrder[i];
        if (card.suit != ledSuit) {
            continue;
        }
        const auto strength = plainStrength(card);
        if (strength && *strength > bestPlainStrength) {
            bestPlainStrength = *strength;
            bestIndex = i;
        }
    }
    return bestIndex;
}

} // namespace qttutorial::games::schafkopf
