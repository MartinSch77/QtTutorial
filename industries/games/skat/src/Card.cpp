// SPDX-License-Identifier: MIT
#include "Card.h"

namespace qttutorial::games::skat {

int cardPoints(Rank rank)
{
    switch (rank) {
    case Rank::Ass:
        return 11;
    case Rank::Ten:
        return 10;
    case Rank::Koenig:
        return 4;
    case Rank::Ober:
        return 3;
    case Rank::Unter:
        return 2;
    case Rank::Nine:
    case Rank::Eight:
    case Rank::Seven:
        return 0;
    }
    return 0;
}

QString suitName(Suit suit)
{
    switch (suit) {
    case Suit::Eichel:
        return QStringLiteral("Eichel");
    case Suit::Gras:
        return QStringLiteral("Gras");
    case Suit::Herz:
        return QStringLiteral("Herz");
    case Suit::Schellen:
        return QStringLiteral("Schellen");
    }
    return {};
}

QString rankName(Rank rank)
{
    switch (rank) {
    case Rank::Seven:
        return QStringLiteral("7");
    case Rank::Eight:
        return QStringLiteral("8");
    case Rank::Nine:
        return QStringLiteral("9");
    case Rank::Ten:
        return QStringLiteral("10");
    case Rank::Unter:
        return QStringLiteral("Unter");
    case Rank::Ober:
        return QStringLiteral("Ober");
    case Rank::Koenig:
        return QStringLiteral("König");
    case Rank::Ass:
        return QStringLiteral("Ass");
    }
    return {};
}

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
    case Rank::Seven:
        return QLatin1Char('7');
    case Rank::Eight:
        return QLatin1Char('8');
    case Rank::Nine:
        return QLatin1Char('9');
    case Rank::Ten:
        return QLatin1Char('T');
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

std::expected<Suit, CardCodeError> suitFromLetter(QChar letter)
{
    switch (letter.toUpper().unicode()) {
    case 'E':
        return Suit::Eichel;
    case 'G':
        return Suit::Gras;
    case 'H':
        return Suit::Herz;
    case 'S':
        return Suit::Schellen;
    default:
        return std::unexpected(CardCodeError::UnknownSuit);
    }
}

std::expected<Rank, CardCodeError> rankFromLetter(QChar letter)
{
    switch (letter.toUpper().unicode()) {
    case '7':
        return Rank::Seven;
    case '8':
        return Rank::Eight;
    case '9':
        return Rank::Nine;
    case 'T':
        return Rank::Ten;
    case 'U':
        return Rank::Unter;
    case 'O':
        return Rank::Ober;
    case 'K':
        return Rank::Koenig;
    case 'A':
        return Rank::Ass;
    default:
        return std::unexpected(CardCodeError::UnknownRank);
    }
}

} // namespace

QString cardCode(const Card& card)
{
    return QString(suitLetter(card.suit)) + QString(rankLetter(card.rank));
}

std::expected<Card, CardCodeError> cardFromCode(const QString& code)
{
    if (code.length() != 2) {
        return std::unexpected(CardCodeError::InvalidLength);
    }
    const auto suit = suitFromLetter(code.at(0));
    if (!suit) {
        return std::unexpected(suit.error());
    }
    const auto rank = rankFromLetter(code.at(1));
    if (!rank) {
        return std::unexpected(rank.error());
    }
    return Card{*suit, *rank};
}

} // namespace qttutorial::games::skat
