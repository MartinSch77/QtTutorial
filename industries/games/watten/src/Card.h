// SPDX-License-Identifier: MIT
#pragma once

#include <QMetaType>
#include <QString>

#include <optional>

namespace qttutorial::watten {

// German-suited deck, 32 cards, no Weli. See industries/games/watten/README.md for why
// this implementation intentionally skips the Weli (the joker-like highest
// trump some regional Watten decks use) and the "Farbe" companion-suit
// mechanic: both are real regional variants, but adding them multiplies the
// ways this one specific ruleset could be gotten wrong.
enum class Suit { Eichel, Gras, Herz, Schellen };

enum class Rank { Sieben, Acht, Neun, Zehn, Unter, Ober, Koenig, Sau };

struct Card {
    Suit suit = Suit::Eichel;
    Rank rank = Rank::Sieben;

    friend bool operator==(const Card&, const Card&) = default;
};

// Sau(Ass) > Koenig > Ober > Unter > 10 > 9 > 8 > 7, the single ranking
// table this implementation uses both for the trump suit and for the suit
// led in a trick -- the "Ass/Koenig swap in trump" variant some tellings of
// Watten use is deliberately not implemented (see README).
[[nodiscard]] int rankValue(Rank rank);

[[nodiscard]] QString suitName(Suit suit);
[[nodiscard]] QString rankName(Rank rank);
[[nodiscard]] QString cardLabel(const Card& card);

// Round-trips suitName()/rankName(): the wire format the network protocol
// uses (see industries/games/watten/README.md) is exactly these strings.
[[nodiscard]] std::optional<Suit> suitFromName(const QString& name);
[[nodiscard]] std::optional<Rank> rankFromName(const QString& name);

} // namespace qttutorial::watten

Q_DECLARE_METATYPE(qttutorial::watten::Card)
Q_DECLARE_METATYPE(qttutorial::watten::Suit)
