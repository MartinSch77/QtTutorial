// SPDX-License-Identifier: MIT
#include "Trump.h"

#include <QtGlobal>

namespace qttutorial::games::skat {

namespace {

int suitOrderIndex(Suit suit)
{
    switch (suit) {
    case Suit::Eichel:
        return 3;
    case Suit::Gras:
        return 2;
    case Suit::Herz:
        return 1;
    case Suit::Schellen:
        return 0;
    }
    return 0;
}

int remainingTrumpRankIndex(Rank rank)
{
    switch (rank) {
    case Rank::Ass:
        return 5;
    case Rank::Ten:
        return 4;
    case Rank::Koenig:
        return 3;
    case Rank::Nine:
        return 2;
    case Rank::Eight:
        return 1;
    case Rank::Seven:
        return 0;
    default:
        return -1;
    }
}

int plainRankIndex(Rank rank, GameType gameType)
{
    switch (rank) {
    case Rank::Ass:
        return 6;
    case Rank::Ten:
        return 5;
    case Rank::Koenig:
        return 4;
    case Rank::Ober:
        // Only reachable in a Grand game: in a Suit game every Ober is
        // trump, so this branch never sees the trump suit's own Ober.
        return gameType == GameType::Grand ? 3 : -1;
    case Rank::Nine:
        return 2;
    case Rank::Eight:
        return 1;
    case Rank::Seven:
        return 0;
    default:
        return -1;
    }
}

} // namespace

bool isTrump(const Card& card, GameType gameType, Suit trumpSuit)
{
    if (gameType == GameType::Null) {
        return false;
    }
    if (card.rank == Rank::Unter) {
        return true;
    }
    if (gameType == GameType::Grand) {
        return false;
    }
    // GameType::Suit
    if (card.rank == Rank::Ober) {
        return true;
    }
    return card.suit == trumpSuit;
}

int cardStrength(const Card& card, GameType gameType, Suit trumpSuit)
{
    if (isTrump(card, gameType, trumpSuit)) {
        if (card.rank == Rank::Unter) {
            return 3000 + suitOrderIndex(card.suit);
        }
        if (card.rank == Rank::Ober) {
            return 2000 + suitOrderIndex(card.suit);
        }
        return 1000 + remainingTrumpRankIndex(card.rank);
    }
    return plainRankIndex(card.rank, gameType);
}

std::size_t trickWinner(const std::vector<Card>& playedInOrder, GameType gameType, Suit trumpSuit)
{
    Q_ASSERT(!playedInOrder.empty());

    bool anyTrump = false;
    for (const Card& card : playedInOrder) {
        if (isTrump(card, gameType, trumpSuit)) {
            anyTrump = true;
            break;
        }
    }

    const Suit ledSuit = playedInOrder.front().suit;
    std::size_t bestIndex = 0;
    int bestStrength = -1;
    for (std::size_t i = 0; i < playedInOrder.size(); ++i) {
        const Card& card = playedInOrder[i];
        const bool cardIsTrump = isTrump(card, gameType, trumpSuit);
        if (anyTrump ? !cardIsTrump : (cardIsTrump || card.suit != ledSuit)) {
            continue;
        }
        const int strength = cardStrength(card, gameType, trumpSuit);
        if (strength > bestStrength) {
            bestStrength = strength;
            bestIndex = i;
        }
    }
    return bestIndex;
}

} // namespace qttutorial::games::skat
