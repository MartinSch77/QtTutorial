// SPDX-License-Identifier: MIT
#include "Card.h"

namespace qttutorial::watten {

int rankValue(Rank rank)
{
    return static_cast<int>(rank);
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
    case Rank::Sieben:
        return QStringLiteral("7");
    case Rank::Acht:
        return QStringLiteral("8");
    case Rank::Neun:
        return QStringLiteral("9");
    case Rank::Zehn:
        return QStringLiteral("10");
    case Rank::Unter:
        return QStringLiteral("U");
    case Rank::Ober:
        return QStringLiteral("O");
    case Rank::Koenig:
        return QStringLiteral("K");
    case Rank::Sau:
        return QStringLiteral("S");
    }
    return {};
}

QString cardLabel(const Card& card)
{
    return rankName(card.rank) + QStringLiteral(" ") + suitName(card.suit);
}

std::optional<Suit> suitFromName(const QString& name)
{
    if (name == QStringLiteral("Eichel")) {
        return Suit::Eichel;
    }
    if (name == QStringLiteral("Gras")) {
        return Suit::Gras;
    }
    if (name == QStringLiteral("Herz")) {
        return Suit::Herz;
    }
    if (name == QStringLiteral("Schellen")) {
        return Suit::Schellen;
    }
    return std::nullopt;
}

std::optional<Rank> rankFromName(const QString& name)
{
    if (name == QStringLiteral("7")) {
        return Rank::Sieben;
    }
    if (name == QStringLiteral("8")) {
        return Rank::Acht;
    }
    if (name == QStringLiteral("9")) {
        return Rank::Neun;
    }
    if (name == QStringLiteral("10")) {
        return Rank::Zehn;
    }
    if (name == QStringLiteral("U")) {
        return Rank::Unter;
    }
    if (name == QStringLiteral("O")) {
        return Rank::Ober;
    }
    if (name == QStringLiteral("K")) {
        return Rank::Koenig;
    }
    if (name == QStringLiteral("S")) {
        return Rank::Sau;
    }
    return std::nullopt;
}

} // namespace qttutorial::watten
