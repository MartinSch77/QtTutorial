// SPDX-License-Identifier: MIT
#include "Deck.h"

#include <algorithm>
#include <array>

namespace qttutorial::maumau {

std::vector<Card> fullDeck()
{
    static constexpr std::array<Suit, 4> suits{Suit::Clubs, Suit::Spades, Suit::Hearts, Suit::Diamonds};
    static constexpr std::array<Rank, 8> ranks{Rank::Seven, Rank::Eight, Rank::Nine, Rank::Ten, Rank::Jack,
                                                Rank::Queen, Rank::King, Rank::Ace};

    std::vector<Card> deck;
    deck.reserve(suits.size() * ranks.size());
    for (Suit suit : suits) {
        for (Rank rank : ranks) {
            deck.push_back(Card{suit, rank});
        }
    }
    return deck;
}

void shuffle(std::vector<Card>& cards, std::mt19937& rng)
{
    std::ranges::shuffle(cards, rng);
}

} // namespace qttutorial::maumau
