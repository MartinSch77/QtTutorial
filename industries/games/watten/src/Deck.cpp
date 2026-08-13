// SPDX-License-Identifier: MIT
#include "Deck.h"

#include <algorithm>
#include <array>

namespace qttutorial::watten {

std::vector<Card> fullDeck()
{
    static constexpr std::array<Suit, 4> suits{Suit::Eichel, Suit::Gras, Suit::Herz, Suit::Schellen};
    static constexpr std::array<Rank, 8> ranks{Rank::Sieben, Rank::Acht,   Rank::Neun, Rank::Zehn,
                                                Rank::Unter,  Rank::Ober,  Rank::Koenig, Rank::Sau};

    std::vector<Card> deck;
    deck.reserve(suits.size() * ranks.size());
    for (Suit suit : suits) {
        for (Rank rank : ranks) {
            deck.push_back(Card{suit, rank});
        }
    }
    return deck;
}

std::vector<Card> shuffledDeck(std::mt19937& rng)
{
    std::vector<Card> deck = fullDeck();
    std::shuffle(deck.begin(), deck.end(), rng);
    return deck;
}

} // namespace qttutorial::watten
