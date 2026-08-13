// SPDX-License-Identifier: MIT
#pragma once

#include <QString>

#include <compare>
#include <expected>

namespace qttutorial::games::skat {

// The four suits of a German-suited (Skat) deck. Names are the German ones
// used at the table; a French-suited player would read Eichel/Gras/Herz/
// Schellen as roughly Clubs/Spades/Hearts/Diamonds.
enum class Suit { Eichel, Gras, Herz, Schellen };

// Ranks of a 32-card Skat deck (no 2-6). "Unter"/"Ober" are the German-deck
// equivalents of Jack/Queen; "Ass" is Ace.
enum class Rank { Seven, Eight, Nine, Ten, Unter, Ober, Koenig, Ass };

struct Card {
    Suit suit;
    Rank rank;

    auto operator<=>(const Card&) const = default;
    bool operator==(const Card&) const = default;
};

[[nodiscard]] int cardPoints(Rank rank);

[[nodiscard]] QString suitName(Suit suit);
[[nodiscard]] QString rankName(Rank rank);

// Compact two-letter code used on the wire and in tests, e.g. "EU" for
// Eichel Unter, "HA" for Herz Ass, "ST" for Schellen Ten.
[[nodiscard]] QString cardCode(const Card& card);

enum class CardCodeError { InvalidLength, UnknownSuit, UnknownRank };
[[nodiscard]] std::expected<Card, CardCodeError> cardFromCode(const QString& code);

} // namespace qttutorial::games::skat
