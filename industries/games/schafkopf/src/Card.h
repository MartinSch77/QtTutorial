// SPDX-License-Identifier: MIT
#pragma once

#include <array>
#include <random>
#include <string_view>
#include <tuple>
#include <vector>

namespace qttutorial::games::schafkopf {

enum class Suit { Eichel, Gras, Herz, Schellen };
enum class Rank { Sieben, Acht, Neun, Zehn, Unter, Ober, Koenig, Ass };

// Deliberately hand-written equality/ordering (rather than a defaulted
// operator<=> pulling in <compare>) -- moc cannot parse libstdc++'s
// <compare>/<concepts> headers when they leak into a Q_OBJECT header that
// transitively includes this one.
struct Card {
    Suit suit;
    Rank rank;

    friend bool operator==(const Card& lhs, const Card& rhs)
    {
        return lhs.suit == rhs.suit && lhs.rank == rhs.rank;
    }

    friend bool operator<(const Card& lhs, const Card& rhs)
    {
        return std::tie(lhs.suit, lhs.rank) < std::tie(rhs.suit, rhs.rank);
    }

    friend bool operator!=(const Card& lhs, const Card& rhs) { return !(lhs == rhs); }
    friend bool operator>(const Card& lhs, const Card& rhs) { return rhs < lhs; }
    friend bool operator<=(const Card& lhs, const Card& rhs) { return !(rhs < lhs); }
    friend bool operator>=(const Card& lhs, const Card& rhs) { return !(lhs < rhs); }
};

[[nodiscard]] constexpr std::array<Suit, 4> allSuits()
{
    return {Suit::Eichel, Suit::Gras, Suit::Herz, Suit::Schellen};
}

[[nodiscard]] constexpr std::array<Rank, 8> allRanks()
{
    return {Rank::Sieben, Rank::Acht, Rank::Neun, Rank::Zehn,
            Rank::Unter, Rank::Ober, Rank::Koenig, Rank::Ass};
}

// The 32-card German-suited deck (4 suits x 8 ranks), in a fixed deterministic
// order -- callers that need randomness go through shuffledDeck() instead.
[[nodiscard]] std::vector<Card> fullDeck();

[[nodiscard]] std::vector<Card> shuffledDeck(std::mt19937& rng);

// Ass=11, Zehn=10, Koenig=4, Ober=3, Unter=2, Neun/Acht/Sieben=0. A full deck's
// points sum to exactly 120, the total contested every hand.
[[nodiscard]] int cardPoints(const Card& card);

[[nodiscard]] std::string_view suitName(Suit suit);
[[nodiscard]] std::string_view rankName(Rank rank);

} // namespace qttutorial::games::schafkopf
