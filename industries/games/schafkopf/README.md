# Schafkopf – Rufspiel

A LAN-multiplayer implementation of Bavarian Schafkopf, specifically the
**Rufspiel** ("call-an-ace") variant -- the single most commonly played form
of the game. This module does **not** implement Wenz or Solo, and does not
implement the full "nobody wants to call" fallback used in real play (a
Wenz/Solo bid, or a lower-stakes "Ramsch"); if all four players pass, this
simplified version just reshuffles and redeals rather than falling back to
another game type. If you know Schafkopf from your own table, treat this as
"Rufspiel only" rather than "Schafkopf in general".

## Rules implemented

- 32-card German-suited deck: Eichel (acorns), Gras (leaves), Herz (hearts),
  Schellen (bells); ranks Ass, Koenig, Ober, Unter, Zehn, Neun, Acht, Sieben.
- Trump order, highest to lowest: the four Ober (Eichel > Gras > Herz >
  Schellen), then the four Unter (same suit order), then all Herz cards by
  rank (Ass, Zehn, Koenig, Neun, Acht, Sieben). Every other card is a plain
  suit card, ranked Ass > Zehn > Koenig > Neun > Acht > Sieben within its suit.
- Card points: Ass=11, Zehn=10, Koenig=4, Ober=3, Unter=2, Neun/Acht/Sieben=0
  (120 points total in the deck).
- Bidding: the player left of the dealer gets first option to call a plain
  suit ace they do not hold, of a suit they hold at least one other
  (non-trump) card of ("Ass muss angespielt werden koennen"). If they
  decline, the option passes around the table once. Whoever holds the
  called ace becomes the calling player's silent partner; the other two
  players are the defending team. If all four players pass, the hand is
  redealt.
- Trick-play: must follow the led suit or trump if holding one; otherwise
  any card may be played. Highest trump wins the trick if any trump was
  played, otherwise the highest card of the led suit wins.
- Scoring: the calling player's team wins ("Spiel") with 61+ of the 120
  card points; 91+ is "Schneider"; taking all 120 (opponents get zero) is
  "Schwarz".

## Architecture

- `src/Card.{h,cpp}`, `src/TrumpOrder.{h,cpp}`, `src/RufspielRules.{h,cpp}` --
  pure, Qt-Core-only rules engine (deck, trump order, trick-winner
  resolution, legal-call/legal-play validation). No `QObject`, no
  networking; this is what `tests/games/schafkopf/` exercises directly.
- `src/SchafkopfProtocol.{h,cpp}` -- the two-character card-id encoding
  (e.g. `"EA"` = Eichel Ass) and message-type string constants shared by the
  engine and every seat's view of it.
- `src/SchafkopfGame.{h,cpp}` -- the authoritative host-side state machine
  (`QObject`). Runs only on the hosting instance: deals, drives bidding,
  validates and sequences trick-play, and scores the hand. It never touches
  a socket -- it only consumes `TableMessage`s via `handleTableMessage()`
  and produces them via the `outgoingMessage()` signal, so it treats a
  decision from a human (relayed by `TableServer`), a remote peer
  (`TableClient`), or a local `Bot` identically.
- `src/SchafkopfClientState.{h,cpp}` -- a seat's-eye view of the hand, built
  purely by replaying `TableMessage`s (`applyMessage()`). Used uniformly for
  the hosting UI's own seat, every bot seat, and every remote client's seat --
  there is exactly one code path for "what does seat N currently see",
  regardless of whether seat N is local or over the network.
- `src/Bot.{h,cpp}` -- fills empty seats. **Deliberately unsophisticated:**
  it calls the first legal ace it finds (or passes if none), and plays a
  legal random card. It does not implement suit-signaling, trump-counting,
  or any other real Schafkopf strategy.
- `src/GameBridge.{h,cpp}` -- the only place in this game that is allowed to
  know about sockets. Owns `TableServer`+`LanAdvertiser` when hosting, or
  `TableClient`+`LanBrowser` when joining; wires their signals to
  `SchafkopfGame`/`SchafkopfClientState`; and exposes everything to QML via
  `QML_ELEMENT` properties (`myHand`, `trick`, `legalCallSuits`, `stage`,
  `handPhase`, ...).
- `qml/` -- `Main.qml` switches between `LobbyScreen` (host/join/discover),
  `BiddingScreen`, `PlayingScreen` (your hand highlighted by legality, the
  trick animating into the center pile, other seats' remaining cards shown
  face-down) and `ResultScreen`. Card faces are hand-drawn with
  `Rectangle`+`Text` (no bundled image assets), colour-coded per suit.

## Network architecture

One instance hosts: it runs `TableServer(3)` (three remaining seats besides
the host's own seat 0) plus `LanAdvertiser` broadcasting discovery beacons
with `gameId = "schafkopf"`. Up to three more instances join via
`TableClient`, either by picking a discovered game from `LanBrowser` or by
typing the host's IP and port directly (both are supported in
`LobbyScreen.qml`, discovery-first). Any of logical seats 1-3 not occupied
by a connected human when the host presses "Start dealing" is filled by a
`Bot` for that hand; a player who disconnects mid-session is replaced by a
bot starting the next hand (not instantly, to avoid mid-trick hand-swaps).

## `TableMessage` protocol (REQ-GAME-04)

All messages use `TableMessage{type, seat, payload}` framed as
newline-delimited JSON by `TableServer`/`TableClient`. `seat` is always a
*logical* seat 0-3 (seat 0 is always the host's own seat; `TableServer`'s
internal 0-based connection-order seats are offset by +1 by `GameBridge`
before reaching `SchafkopfGame`).

| type | direction | payload | meaning |
| --- | --- | --- | --- |
| `welcome` | host -> joining client | `{seat}` | Assigns the client's logical seat. |
| `handDealt` | host -> each seat individually | `{dealer, hand: [cardId, ...]}` | That seat's own 8 cards for the new hand. Never broadcast, since each seat's hand is private. |
| `biddingTurn` | host -> all | `{seat}` | Whose turn it is to call or pass. |
| `call` | seat -> host | `{suit}` (one of `"E"`/`"G"`/`"S"`) | Announces a Rufspiel call. |
| `pass` | seat -> host | `{}` | Declines to call. |
| `callMade` | host -> all | `{callerSeat, suit}` | The call that ended bidding. The partner's identity is deliberately *not* included -- it stays secret until the called ace is played or the trick makes it obvious. |
| `redeal` | host -> all | `{}` | All four seats passed; a new hand is about to be dealt. |
| `trickTurn` | host -> all | `{seat}` | Whose turn it is to play a card. |
| `playCard` | seat -> host | `{cardId}` | Plays one card from that seat's hand. |
| `cardPlayed` | host -> all | `{seat, cardId}` | Broadcasts the card that was just played. |
| `trickCompleted` | host -> all | `{winnerSeat, points}` | The just-finished trick's winner and card-point value. |
| `handCompleted` | host -> all | `{callerSeat, partnerSeat, calledSuit, callerTeamPoints, opponentTeamPoints, callerTeamWon, schneider, schwarz}` | Final scoring for the hand, including the now-revealed partner. |
| `error` | host -> offending seat | `{reason}` | An illegal call/pass/play was rejected; that seat's turn is unchanged. |

Card ids are two characters: a suit letter (`E`/`G`/`H`/`S`) followed by a
rank letter (`7`/`8`/`9`/`Z` for Zehn/`U`/`O`/`K`/`A`), e.g. `"HU"` = Herz
Unter.

## Build & run

From the repository root:

```sh
cmake -S . -B build
cmake --build build --target schafkopf
./build/games/schafkopf/schafkopf
```

Run several instances on the same machine (or LAN) to play with more than
one human seat; empty seats are filled by bots.

## Tests

`tests/games/schafkopf/` covers the rules engine directly, independent of
any UI or networking: trump-order ranking (`test_trump_order.cpp`),
trick-winner resolution across plain-suit, trump-in-plain-suit and all-trump
tricks (`test_trick_winner.cpp`), Rufspiel call legality
(`test_rufspiel_rules_call.cpp`), follow-suit legal-play validation
(`test_rufspiel_rules_play.cpp`), and card-point scoring, including that a
full deck sums to exactly 120 (`test_card_scoring.cpp`).
