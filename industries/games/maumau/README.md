# Mau-Mau

A LAN-multiplayer implementation of **Mau-Mau**, the German household card game played with a
32-card Skat-pattern deck. This implements the core, widely-agreed ruleset only — the rules
almost every German family agrees on — and deliberately leaves out the many local house-rule
extras (e.g. treating Ace as a skip card, stacking multiple 7s, a "no last card announced" penalty,
point-scoring across rounds). If the rules below don't match how your family plays, that's
expected: Mau-Mau has significant regional variation, and this module documents exactly which
single, common variant it implements so you can compare it against your own rules rather than
being surprised by an undocumented house rule.

## Rules implemented

- **Deck**: 32 cards, French-suited (Clubs/Spades/Hearts/Diamonds) purely for simplicity — this
  changes none of the game's rules, only the pictures. Ranks: 7, 8, 9, 10, Jack, Queen, King, Ace.
- **Players**: 2–4. The table always has exactly 4 seats; any seat with no human connected when
  the host starts the round is filled by a basic bot (see below), so a lone host can still play a
  full round.
- **Deal**: 5 cards to each seat, dealt round-robin. The remaining cards form the draw pile; its
  top card is flipped to start the discard pile.
- **A turn**: play one card from your hand that matches the discard pile's top card by suit or by
  rank, or draw one card from the draw pile. **Our deliberate choice**: drawing always ends your
  turn, even if the card you drew would have been playable. (The alternative — "draw, and if it's
  playable you may immediately play it" — is an equally common house rule; we picked the simpler,
  single-action version and are naming that choice explicitly here rather than leaving it
  ambiguous.)
- **7 (Seven)**: the next player must draw two cards, and their turn is skipped entirely — they do
  not also get to play. **Our deliberate choice**: non-stacking. Playing a second 7 in response
  does not add to a pending draw count; each 7 only ever affects the single next player, for two
  cards.
- **8 (Eight)**: the next player's turn is skipped entirely (no draw).
- **Jack (Bube)**: always playable, on any top card, regardless of suit/rank/any pending wish. The
  player who plays it must immediately wish a suit; the next player must play a card of that
  suit, or another Jack, regardless of the Jack's own suit. Once that next player has played
  (whether they satisfied the wish or played a new Jack — which starts a fresh wish), the wish is
  consumed: any player after that is judged normally against the new top card, not the old wish.
  The wish never re-applies to the Jack's own player later.
- **The very first card flipped to start the discard pile never triggers a special effect** — even
  if it's a 7, 8, or Jack. Nobody played it, so there's no preceding player to skip, no next
  player mid-turn to force a draw onto, and no player to have wished a suit. This is a deliberate,
  documented edge-case choice, not an oversight.
- **Winning**: the first player to empty their hand wins the round immediately. There is no
  cross-round scoring in this implementation — a nice-to-have we chose not to block finishing the
  core game on.
- **2-player skip behavior**: with only 2 seats active, an 8 (or the skip after a 7's forced draw)
  "skips" the only opponent and lands back on the same player, who effectively gets a second
  consecutive turn. This falls out naturally from the turn-order math (seat advances are modulo
  the seat count) rather than being special-cased, and is covered by
  `tests/games/maumau/test_maumau_game.cpp`.

## The bot

Any of the 4 seats not occupied by a connected human when the host clicks "Start Game" is played
by a simple, intentionally **basic/non-strategic** bot: on its turn it plays the first legal card
it finds in hand order, or draws if it has none. If it plays a Jack, it wishes whichever suit it
now holds the most cards of (a very small heuristic, not real strategy). The bot exists to make
the game playable and testable with fewer than 4 humans, not to be a challenging opponent.

## Architecture

- `src/Card.{h,cpp}`, `src/Deck.{h,cpp}` — the card model. Pure C++23, no Qt dependency, so it can
  be reused, tested, or read without pulling in Qt at all.
- `src/MoveValidator.{h,cpp}` — pure legality checks (`isLegalPlay`) and the special-card effect
  lookup (`effectOf`), also Qt-free.
- `src/CardCodec.{h,cpp}` — the one place `Card` meets `QJsonObject`, converting to/from the JSON
  shape used in `TableMessage::payload`.
- `src/MauMauGame.{h,cpp}` — the authoritative `QObject` state machine: deals, tracks turn order,
  applies special-card effects, detects the win, and runs the built-in bot. It only ever consumes
  and produces `TableMessage`s (via `handleMessage()`, and the `seatMessage`/`broadcastMessage`
  signals) — it has no idea whether a given seat's messages come from a human at the host, a
  network peer, or the bot loop calling straight into it. All four of the above make up
  `maumau_lib` and are unit-tested independent of Qt Quick and independent of
  `TableServer`/`TableClient` (REQ-GAME-02/03).
- `src/GameController.{h,cpp}` — the QML-facing glue (`QML_ELEMENT`). Owns the LAN transport
  (`TableServer`, `TableClient`, `LanAdvertiser`, `LanBrowser`) and, when hosting, the
  `MauMauGame` instance; translates `TableMessage`s into QML-bindable properties and back. Kept
  outside `maumau_lib`, alongside `main.cpp`, the same way `framework-tour/02-qml-quick-basics`
  keeps its QML-registered backend classes out of its own logic library.
- `qml/` — the UI: `LobbyPage.qml` (host/join/discover), `TablePage.qml` (the table itself: hand,
  discard/draw piles, opponent seat strip, turn indicator, notifications), `PlayingCard.qml`
  (hand-painted card face/back — `Rectangle` + `Text`, no image assets), `SuitPickerPopup.qml`
  (the suit-wish popup after playing a Jack).

### Why the host also runs a `TableClient`

Rather than special-casing "seat 0 is the host, handle its moves in-process" throughout
`GameController`, `hostGame()` opens a `TableClient` connected to `127.0.0.1` on its own
`TableServer`'s port right after starting to listen. The host's own UI then drives its seat
through the exact same `TableMessage` send/receive path a remote peer uses — no branching in
`GameController` between "local" and "networked" seats. This relies on the loopback connection
completing before any real remote peer can react to the just-sent discovery beacon and dial in
(a LAN round-trip plus a human clicking "Join" is orders of magnitude slower than a loopback TCP
handshake), which is a deliberate, documented timing assumption rather than a race condition we
consider a bug.

## Networking / `TableMessage` protocol (REQ-GAME-04)

Every message uses the shared `TableMessage{type, seat, payload}` envelope
(`industries/games/common/src/TableMessage.h`). `gameId = "maumau"` in the `LanBeacon` used for discovery.
The host always occupies seat 0; up to 3 more seats are numbered 1–3 in the order `TableServer`
accepts their connections (the host's own loopback connection is always first).

| `type` | Direction | `payload` | Meaning |
| --- | --- | --- | --- |
| `welcome` | server → one client (`sendTo`) | `{ "seatsTotal": 4 }` | Sent immediately on connect (`TableServer::seatConnected`); `TableMessage::seat` tells the client which seat it was assigned. |
| `hand` | server → one client (`sendTo`) | `{ "cards": [ { "rank": "Jack", "suit": "Hearts" }, ... ] }` | That seat's current hand. Sent after every deal and after every move affecting that seat's hand. |
| `public_state` | server → all (`broadcast`) | `{ "topCard": {"rank":..,"suit":..}, "wishedSuit": "Hearts" \| null, "handCounts": [n0,n1,n2,n3], "currentSeat": int, "drawPileCount": int, "phase": "lobby" \| "playing" \| "round_over", "winnerSeat": int \| null }` | Everything every seat is allowed to see about the table. |
| `play_card` | client → server | `{ "rank": "Jack", "suit": "Clubs", "wish": "Hearts" }` (`wish` present only for a Jack) | A play attempt for the sender's seat. |
| `draw_card` | client → server | `{}` | A draw request for the sender's seat. |
| `notice` | server → all (`broadcast`) | `{ "kind": "draw_two" \| "skip", "seat": int }` | A transient, human-readable event (used by the UI's toast banner) — which seat had to draw two, or whose turn was skipped. |
| `action_error` | server → one client (`sendTo`) | `{ "reason": "not_your_turn" \| "illegal_card" \| "card_not_in_hand" \| "malformed_card" \| "missing_wish" \| "round_over" \| "unknown_message_type" }` | Rejection of a `play_card`/`draw_card` that failed validation. |

Card JSON shape (used inside `hand`, `public_state.topCard`, `play_card`): `{"rank": <one of
"Seven","Eight","Nine","Ten","Jack","Queen","King","Ace">, "suit": <one of
"Clubs","Spades","Hearts","Diamonds">}` — see `src/CardCodec.h`.

## Build & run

From the repository root:

```sh
cmake -S . -B build
cmake --build build --target maumau
./build/games/maumau/maumau
```

Run a second (or third, fourth) instance on the same LAN (or the same machine, for local testing)
to join: use "Join a game on this LAN" if discovery finds it, or "Join by IP" with the host's
address and the port shown by the host (visible via the LAN beacon; for same-machine testing use
`127.0.0.1` and whatever port the host's `TableServer` picked).

## Tests

`tests/games/maumau/`:

- `test_deck.cpp` — the deck has exactly 32 cards, 4 suits of 8 ranks each with no duplicates, and
  shuffling produces a permutation of the same cards.
- `test_move_validator.cpp` — suit-match, rank-match, and mismatch legality; a Jack is always
  legal even under a pending wish; a pending wished suit overrides the top card for every other
  rank; the 7/8/Jack → effect mapping.
- `test_maumau_game.cpp` — the full state machine via `MauMauGame::startNewGameWithFixedDeck()`
  (deals from a caller-supplied, pre-arranged deck instead of a real shuffle, so hands and the
  starting discard card are deterministic): a 7 forces exactly the next player to draw two and
  skips them; an 8 skips the next player without drawing; with only 2 seats, that skip lands back
  on the same player; a Jack's wish binds only the very next play and is gone by the play after
  that; playing against an unmet wish, out of turn, or a card not in hand are all rejected with
  the matching `action_error` reason; emptying your hand ends the round and fires `roundOver` for
  the correct seat.
