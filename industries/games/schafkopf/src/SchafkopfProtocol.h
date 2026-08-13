// SPDX-License-Identifier: MIT
#pragma once

#include "Card.h"

#include <QString>

#include <optional>

namespace qttutorial::games::schafkopf {

// Phases of one Rufspiel hand, mirrored identically on the host (SchafkopfGame)
// and on every seat's view of it (SchafkopfClientState).
enum class Phase { Bidding, Playing, HandComplete };

// The TableMessage "type" strings this game puts on the wire. See the
// README for the full protocol table (who sends what, and what "payload"
// looks like for each).
namespace MessageType {
inline constexpr auto Welcome = "welcome";
inline constexpr auto HandDealt = "handDealt";
inline constexpr auto BiddingTurn = "biddingTurn";
inline constexpr auto Call = "call";
inline constexpr auto Pass = "pass";
inline constexpr auto CallMade = "callMade";
inline constexpr auto Redeal = "redeal";
inline constexpr auto TrickTurn = "trickTurn";
inline constexpr auto PlayCard = "playCard";
inline constexpr auto CardPlayed = "cardPlayed";
inline constexpr auto TrickCompleted = "trickCompleted";
inline constexpr auto HandCompleted = "handCompleted";
inline constexpr auto Error = "error";
} // namespace MessageType

// Two-character wire ids, e.g. "EA" = Eichel Ass, "HU" = Herz Unter,
// "SZ" = Schellen Zehn. Compact, human-readable in logs, and trivial to
// round-trip through QJsonValue.
[[nodiscard]] QString cardToId(const Card& card);
[[nodiscard]] std::optional<Card> cardFromId(const QString& id);

[[nodiscard]] QString suitToId(Suit suit);
[[nodiscard]] std::optional<Suit> suitFromId(const QString& id);

} // namespace qttutorial::games::schafkopf
