// SPDX-License-Identifier: MIT
#include "Card.h"

#include <algorithm>

namespace qttutorial::games::schafkopf {

std::vector<Card> fullDeck()
{
    std::vector<Card> deck;
    deck.reserve(32);
    for (Suit suit : allSuits()) {
        for (Rank rank : allRanks()) {
            deck.push_back(Card{suit, rank});
        }
    }
    return deck;
}

std::vector<Card> shuffledDeck(std::mt19937& rng)
{
    std::vector<Card> deck = fullDeck();
    std::ranges::shuffle(deck, rng);
    return deck;
}

int cardPoints(const Card& card)
{
    switch (card.rank) {
    case Rank::Ass:
        return 11;
    case Rank::Zehn:
        return 10;
    case Rank::Koenig:
        return 4;
    case Rank::Ober:
        return 3;
    case Rank::Unter:
        return 2;
    case Rank::Neun:
    case Rank::Acht:
    case Rank::Sieben:
        return 0;
    }
    return 0;
}

std::string_view suitName(Suit suit)
{
    switch (suit) {
    case Suit::Eichel:
        return "Eichel";
    case Suit::Gras:
        return "Gras";
    case Suit::Herz:
        return "Herz";
    case Suit::Schellen:
        return "Schellen";
    }
    return "";
}

std::string_view rankName(Rank rank)
{
    switch (rank) {
    case Rank::Sieben:
        return "Sieben";
    case Rank::Acht:
        return "Acht";
    case Rank::Neun:
        return "Neun";
    case Rank::Zehn:
        return "Zehn";
    case Rank::Unter:
        return "Unter";
    case Rank::Ober:
        return "Ober";
    case Rank::Koenig:
        return "Koenig";
    case Rank::Ass:
        return "Ass";
    }
    return "";
}

} // namespace qttutorial::games::schafkopf
