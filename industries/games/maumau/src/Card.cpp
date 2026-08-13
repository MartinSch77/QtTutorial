// SPDX-License-Identifier: MIT
#include "Card.h"

namespace qttutorial::maumau {

std::string_view toString(Suit suit)
{
    switch (suit) {
    case Suit::Clubs:
        return "Clubs";
    case Suit::Spades:
        return "Spades";
    case Suit::Hearts:
        return "Hearts";
    case Suit::Diamonds:
        return "Diamonds";
    }
    return "Clubs";
}

std::string_view toString(Rank rank)
{
    switch (rank) {
    case Rank::Seven:
        return "Seven";
    case Rank::Eight:
        return "Eight";
    case Rank::Nine:
        return "Nine";
    case Rank::Ten:
        return "Ten";
    case Rank::Jack:
        return "Jack";
    case Rank::Queen:
        return "Queen";
    case Rank::King:
        return "King";
    case Rank::Ace:
        return "Ace";
    }
    return "Seven";
}

std::string toDisplayString(const Card& card)
{
    return std::string(toString(card.rank)) + " of " + std::string(toString(card.suit));
}

std::optional<Suit> suitFromString(std::string_view text)
{
    if (text == "Clubs") return Suit::Clubs;
    if (text == "Spades") return Suit::Spades;
    if (text == "Hearts") return Suit::Hearts;
    if (text == "Diamonds") return Suit::Diamonds;
    return std::nullopt;
}

std::optional<Rank> rankFromString(std::string_view text)
{
    if (text == "Seven") return Rank::Seven;
    if (text == "Eight") return Rank::Eight;
    if (text == "Nine") return Rank::Nine;
    if (text == "Ten") return Rank::Ten;
    if (text == "Jack") return Rank::Jack;
    if (text == "Queen") return Rank::Queen;
    if (text == "King") return Rank::King;
    if (text == "Ace") return Rank::Ace;
    return std::nullopt;
}

} // namespace qttutorial::maumau
