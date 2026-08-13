// SPDX-License-Identifier: MIT
#include "Deck.h"

#include <algorithm>

namespace qttutorial::games::skat {

std::vector<Card> buildDeck()
{
    static constexpr std::array<Suit, 4> suits{Suit::Eichel, Suit::Gras, Suit::Herz, Suit::Schellen};
    static constexpr std::array<Rank, 8> ranks{Rank::Seven, Rank::Eight, Rank::Nine, Rank::Ten,
                                                Rank::Unter, Rank::Ober, Rank::Koenig, Rank::Ass};
    std::vector<Card> deck;
    deck.reserve(suits.size() * ranks.size());
    for (Suit suit : suits) {
        for (Rank rank : ranks) {
            deck.push_back(Card{suit, rank});
        }
    }
    return deck;
}

Deal dealHand(std::mt19937& rng)
{
    std::vector<Card> deck = buildDeck();
    std::shuffle(deck.begin(), deck.end(), rng);

    Deal deal;
    std::size_t cursor = 0;
    for (auto& hand : deal.hands) {
        hand.assign(deck.begin() + static_cast<long>(cursor), deck.begin() + static_cast<long>(cursor) + 10);
        cursor += 10;
    }
    deal.skat.assign(deck.begin() + static_cast<long>(cursor), deck.begin() + static_cast<long>(cursor) + 2);
    return deal;
}

} // namespace qttutorial::games::skat
