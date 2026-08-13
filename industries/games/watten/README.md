# Watten (Tyrolean-style, 4-player partnership) – simplified digital core

Watten ("Watter" in dialect) is a real Alpine trick-taking card game played
across Tyrol, South Tyrol, Bavaria and Austria, with **significant regional
rule variation** in card ranking, deck size and, above all, the secret
partner-signaling tradition ("Zeichengeben"). This directory implements
**one specific, documented variant** of it, not "the" rules of Watten.
Players who know a different table's rules should expect differences.

## Which variant this is, exactly

- **4 players, 2 fixed partnerships**: seats 0+2 vs. seats 1+3 (partners sit
  opposite each other at the table, as in the real game).
- **32-card German-suited deck, no Weli.** Suits: Eichel, Gras, Herz,
  Schellen. Ranks: 7, 8, 9, 10, Unter, Ober, König, Sau (Ass). This
  implementation deliberately **omits the Weli** (the joker-like card some
  regional 33-card decks use as the single highest trump) — it is a real,
  documented variant, but skipping it removes one whole axis of "did we get
  the ranking table right".
- **Ranking table (used identically for the trump suit and for the suit
  led):** Sau (Ass) > König > Ober > Unter > 10 > 9 > 8 > 7. Some regional
  tellings swap Ass and König specifically within the trump suit, or give
  the two suits sharing the trump's color ("Farbe"/companion suits)
  trump-like status; this implementation uses the single, simpler ranking
  table above and does **not** implement the companion-suit mechanic at
  all — there is exactly one trump suit per hand, decided as follows.
- **Trump is decided by the opening lead.** Whoever leads the first trick of
  a hand (the "Ausspieler") fixes trump for that whole hand as the suit of
  their led card. There is no bidding.
- **No obligation to follow suit.** Any card may be played to any trick —
  a real, common Watten trait (unlike, say, Schafkopf). The trick's winner
  is the highest trump played, if any trump was played at all; otherwise
  the highest card of the suit that was led. A card that is neither trump
  nor the led suit can never win a trick.
- **Dealing and hand length:** each hand, all 4 seats are dealt 5 cards from
  a freshly shuffled 32-card deck (20 of the 32 cards are used; the
  remaining 12 sit out that hand — no widow/discard mechanic is modeled).
  A hand therefore has at most 5 tricks. The trick's winner leads the next
  trick.
- **Scoring ("Bummerl"):** each hand is worth exactly one point. The first
  team to win 3 of that hand's (at most 5) tricks takes the point
  immediately — with only 5 tricks split between 2 teams, one side always
  reaches 3 by the 5th trick at the latest, so this never leaves a hand
  undecided. The first team to reach **11 points** wins the Bummerl.
- **Dealer/leader rotation:** the Ausspieler role rotates one seat clockwise
  (`(previousDealer + 1) % 4`) after every hand.

## Explicitly out of scope: Zeichengeben (secret signaling)

Real Tyrolean Watten is famous for **Zeichengeben** — partners secretly
signaling what's in their hand to each other via winks, taps, foot
positions, or set phrases, all while trying not to get caught by the
opposing team. This is a social, physical-gesture mechanic that has no fair
digital equivalent: a bot cannot "wink", and a remote human partner over a
LAN connection has no analogous covert channel short of a second, out-of-band
communication tool this app does not provide. **This implementation does not
attempt to model, allow, or substitute for Zeichengeben in any way.** That is
a real and significant simplification — Zeichengeben materially changes how
the real game is played and reasoned about — so this digital version should
be understood as the trick-taking *core* of Watten, not an equivalent
experience to playing it at a table with people you can watch.

## Architecture (REQ-GAME-02)

- `src/Card.{h,cpp}` — `Suit`/`Rank` enums, the single ranking table
  (`rankValue`), and string round-tripping (`suitName`/`rankName` and their
  inverses) used both for QML display and the wire protocol below.
- `src/Deck.{h,cpp}` — the unshuffled 32-card deck and a seeded
  Fisher-Yates shuffle.
- `src/TrumpRanking.{h,cpp}` — `trickWinnerSeat()`, the pure function that
  resolves one trick (highest trump, else highest of the suit led).
- `src/BummerlScore.{h,cpp}` — `Team`/`teamForSeat()` and the Bummerl point
  counter, independent of trick-taking so it can be tested on its own.
- `src/WattenGame.{h,cpp}` — the `QObject` state machine: dealing, trump
  determination from the opening lead, trick sequencing with no
  follow-suit obligation, and hand/Bummerl scoring. Depends only on
  `Qt6::Core` — no `Qt6::Network`, no UI types.
- `src/WattenBot.{h,cpp}` — the "empty seat" bot: plays a uniformly random
  legal card whenever it is its turn. It does not evaluate card strength,
  help its partner, or bluff. This is a placeholder opponent, not an AI
  with any real Watten skill — documented honestly rather than oversold.

All five of the above compile into `watten_lib` and are covered by
`tests/games/watten/` (REQ-GAME-03), independent of the UI and of
`games_common_lib`.

`src/WattenTableController.{h,cpp}` is the one class allowed to depend on
both `watten_lib` and `games_common_lib`: it is the QML-exposed bridge that
translates `WattenGame`'s C++ signals into the network protocol below when
hosting, and turns incoming `TableMessage`s back into UI state (and, on the
host, into `WattenGame` calls) — see `industries/games/watten/src/WattenTableController.cpp`.
`qml/*.qml` is the Qt Quick UI: `Main.qml` (table layout, 4 seats with
partners opposite each other, trump/score/turn indicators), `CardFace.qml`
(a hand-painted card face — `Rectangle` + `Canvas`-drawn suit glyph, no
image assets), `PlayerHand.qml`, `TrickArea.qml`, `LobbyView.qml` (host/join
screen, including LAN discovery).

## Networking and protocol (REQ-GAME-04)

The hosting process runs `games::common::TableServer(4)` and always plays
seat 0. Up to 3 other seats are filled by processes that run
`games::common::TableClient` and connect to the host (manual IP:port entry,
or discovery via `games::common::LanBrowser` with `gameId = "watten"`,
matching what the host advertises via `LanAdvertiser`). Any of the 4 seats
not filled by a connected human by the time the host starts the table is
played by a local `WattenBot` that the host drives directly and entirely
outside the network protocol — bots are never network seats.

This is a **host-authoritative** design: only the host process ever
constructs a `WattenGame`. Joining clients are thin: they render whatever
the host broadcasts and send `play_card` requests; the host is the sole
validator. All messages are `qttutorial::games::common::TableMessage{type,
seat, payload}` as newline-delimited JSON, exactly as defined in
`industries/games/common/src/TableMessage.h`.

| `type`          | Direction        | `payload`                                              | Notes |
|------------------|-------------------|----------------------------------------------------------|-------|
| `welcome`        | host → one client | `{"seat": <int>}`                                         | Sent once, right after `TableServer::seatConnected`; tells the client which seat it was assigned. |
| `hand_dealt`     | host → one client | `{"cards": [{"suit": "...", "rank": "..."}, ...]}`         | Private: only that seat's own 5 cards. Never broadcast. |
| `hand_started`   | host → all         | `{"dealerSeat": <int>}`                                    | New hand; announces the Ausspieler for this hand. |
| `trump_decided`  | host → all         | `{"suit": "Eichel"\|"Gras"\|"Herz"\|"Schellen"}`            | Emitted once the opening lead is played. |
| `card_played`    | host → all         | `{"seat": <int>, "suit": "...", "rank": "..."}`             | Public: a card just entered the current trick. |
| `turn_changed`   | host → all         | `{"seat": <int>}`                                          | Whose turn it is now. |
| `trick_won`      | host → all         | `{"seat": <int>, "trickIndex": <int>}`                     | Trick resolved; `seat` is the winner. |
| `hand_won`       | host → all         | `{"team": "A"\|"B", "scoreA": <int>, "scoreB": <int>}`      | Hand resolved; running Bummerl scores. |
| `bummerl_won`    | host → all         | `{"team": "A"\|"B"}`                                       | Sent once a team reaches 11 points. |
| `play_card`      | client → host      | `{"suit": "...", "rank": "..."}`                            | `TableMessage::seat` is the sender's assigned seat. |
| `invalid_move`   | host → one client  | `{"reason": "..."}`                                        | Sent back if a `play_card` request was illegal (not that seat's turn, or not in that seat's hand). |

`"suit"`/`"rank"` string values are exactly `suitName()`/`rankName()` from
`src/Card.cpp` (e.g. `"Herz"`, `"Sau"` is written as rank string `"S"` —
see `rankName`/`rankFromName` for the full short-code table).

## Build & run

From the repository root:

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_GAMES=ON
cmake --build build --target watten
./build/games/watten/watten
```

Pick "Host" to start a table (any of seats 1-3 not joined by another
`watten` instance on the LAN within a few seconds of pressing "Host" is
filled by `WattenBot`), or "Search LAN"/"Join by address" to join one.

## Tests (REQ-GAME-03)

`tests/games/watten/` covers, independent of the UI and of
`games_common_lib`:

- `test_trump_ranking.cpp` — the ranking table (`rankValue`) and
  `trickWinnerSeat()`, including a trick with no trump played and one where
  trump was played by a seat other than the one who led.
- `test_deal_and_trump.cpp` — that `startNewHand()` deals 5 cards per seat
  from a 32-card deck with no duplicates, and that trump is fixed to the
  suit of whichever seat's card opens the first trick (and only the first).
- `test_bummerl_score.cpp` — point accumulation per team and winning the
  Bummerl at the 11-point target, including that scoring stops advancing
  once won.
- `test_watten_game_flow.cpp` — a full scripted hand through `WattenGame`
  confirming trick sequencing, no-follow-suit legality, and that a team
  reaching 3 trick wins ends the hand immediately.
