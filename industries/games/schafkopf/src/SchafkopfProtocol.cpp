// SPDX-License-Identifier: MIT
#include "SchafkopfProtocol.h"

namespace qttutorial::games::schafkopf {

namespace {

QChar suitLetter(Suit suit)
{
    switch (suit) {
    case Suit::Eichel:
        return QLatin1Char('E');
    case Suit::Gras:
        return QLatin1Char('G');
    case Suit::Herz:
        return QLatin1Char('H');
    case Suit::Schellen:
        return QLatin1Char('S');
    }
    return QLatin1Char('?');
}

QChar rankLetter(Rank rank)
{
    switch (rank) {
    case Rank::Sieben:
        return QLatin1Char('7');
    case Rank::Acht:
        return QLatin1Char('8');
    case Rank::Neun:
        return QLatin1Char('9');
    case Rank::Zehn:
        return QLatin1Char('Z');
    case Rank::Unter:
        return QLatin1Char('U');
    case Rank::Ober:
        return QLatin1Char('O');
    case Rank::Koenig:
        return QLatin1Char('K');
    case Rank::Ass:
        return QLatin1Char('A');
    }
    return QLatin1Char('?');
}

} // namespace

QString cardToId(const Card& card)
{
    return QString(suitLetter(card.suit)) + QString(rankLetter(card.rank));
}

std::optional<Card> cardFromId(const QString& id)
{
    if (id.size() != 2) {
        return std::nullopt;
    }
    const auto suit = suitFromId(id.left(1));
    if (!suit) {
        return std::nullopt;
    }
    for (Rank rank : allRanks()) {
        if (rankLetter(rank) == id.at(1)) {
            return Card{*suit, rank};
        }
    }
    return std::nullopt;
}

QString suitToId(Suit suit)
{
    return QString(suitLetter(suit));
}

std::optional<Suit> suitFromId(const QString& id)
{
    if (id.size() != 1) {
        return std::nullopt;
    }
    for (Suit suit : allSuits()) {
        if (suitLetter(suit) == id.at(0)) {
            return suit;
        }
    }
    return std::nullopt;
}

} // namespace qttutorial::games::schafkopf
