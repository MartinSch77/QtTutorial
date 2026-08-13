// SPDX-License-Identifier: MIT
#pragma once

// moc's own C++ parser (used transitively whenever a Q_OBJECT header includes this one, e.g.
// MauMauGame.h) chokes on libstdc++'s <concepts> internals pulled in by <compare>; it never
// needs this include itself, so skip it during moc's pass.
#ifndef Q_MOC_RUN
#include <compare>
#endif
#include <optional>
#include <string>
#include <string_view>

namespace qttutorial::maumau {

enum class Suit { Clubs, Spades, Hearts, Diamonds };
enum class Rank { Seven, Eight, Nine, Ten, Jack, Queen, King, Ace };

struct Card {
    Suit suit;
    Rank rank;

    friend auto operator<=>(const Card&, const Card&) = default;
    friend bool operator==(const Card&, const Card&) = default;
};

[[nodiscard]] std::string_view toString(Suit suit);
[[nodiscard]] std::string_view toString(Rank rank);
[[nodiscard]] std::string toDisplayString(const Card& card);

// Parsers for the same strings toString() produces, used to decode cards coming back over the
// network. Return std::nullopt rather than throwing so message decoding can reject bad input.
[[nodiscard]] std::optional<Suit> suitFromString(std::string_view text);
[[nodiscard]] std::optional<Rank> rankFromString(std::string_view text);

} // namespace qttutorial::maumau
