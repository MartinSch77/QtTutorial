# Skat

Implements **Skat**, the German 3-player trick-taking card game, for a table
of up to 4 LAN players (REQ-GAME-05). No regional variant is targeted beyond
the rule choices called out under "Simplifications" below.

## The 4-seat, 3-active table

A real Skat table seats up to 4 players; exactly 3 are *active* in any given
hand and the 4th sits out, with the dealer rotating clockwise each hand. This
is authentic Skat table practice for a 4-player group, not a corner cut: the
dealer is the one who sits out, deals, and rejoins the active rotation on
their next turn to deal. This game always runs with `TableServer(4)`: up to
4 people can connect, and whichever seats are not occupied by a connected
human are played by a basic bot (see "Bot skill level"), so a hand always has
exactly 4 seats -- 3 active, 1 sitting out -- regardless of how many humans
actually joined. If only 3 humans are at the table, the 4th (bot) seat still
takes its turn sitting out and dealing like everyone else.

## Suit naming

German-suited, 32-card deck (7, 8, 9, 10, Unter, Ober, König, Ass in each of
Eichel, Gras, Herz, Schellen). "Unter"/"Ober" are this deck's equivalents of
Jack/Queen. In the QML card faces, each suit is drawn with a French-suit
glyph purely as a rendering shortcut that any font supports reliably:
Eichel~♣, Gras~♠, Herz~♥, Schellen~♦. This is a display choice only; the
rules below are entirely German-suited.

## Simplifications

This tutorial module deliberately trades a few points of authenticity for a
much smaller, more testable rule set. Each deviation from official
tournament Skat is listed here so a player can check the digital rules
against the physical ones they know (REQ-GAME-05):

- **Bidding ("Reizen")**: real Skat bidding is a two-player-at-a-time
  exchange where the higher-ranked player silently states a value from a
  fixed value ladder and the lower-ranked one says "yes" or passes. This
  game instead runs a simple round-robin numeric auction (`Bidding` in
  `src/Bidding.h`): each of the 3 active players, in turn, either raises
  strictly above the current highest bid (starting from a baseline of 18)
  or passes for good, until only one player has not passed -- that player
  is the declarer ("Alleinspieler"), committed to a game worth at least
  their final bid (or the baseline, if the other two passed before anyone
  ever bid).
- **Trump order**: the 4 Unters are trump in every game type but Null,
  ranked Eichel > Gras > Herz > Schellen, same as official Skat. In a
  **Suit** game, this tutorial additionally makes **all 4 Obers** trump
  (ranked the same suit order, below the Unters), followed by the trump
  suit's own remaining cards (Ass > 10 > König > 9 > 8 > 7). Official Skat
  only makes the *trump suit's own* Ober trump -- the other three Obers stay
  ordinary plain-suit cards (ranked between König and 9). This tutorial's
  rule is simpler to reason about and test (four fixed non-Unter trump
  groups instead of "one suit's Ober is special"), at the cost of matching
  the official ladder exactly. In a **Grand** game only the 4 Unters are
  trump, in both rule sets.
- **Game-value scoring**: official Skat computes a game value from a base
  value per game type, multiplied by "Spitzen" (consecutive top trumps) and
  level modifiers (Hand, Schneider angesagt, Schwarz, etc.), with separate
  under-bid/over-bid penalties. None of that is implemented. This game
  instead reports a plain win/lose plus the raw point split, e.g. "declarer
  won with 68/120 points" -- see `SkatGame::declarerWon()`. The bid amount
  from the simplified auction only sets the *minimum* game value the
  declarer must reach; it does not otherwise affect scoring.
- **Null games are not implemented.** A Null game's win condition (zero
  tricks taken, not a point-count threshold) is different enough from
  Suit/Grand that it deserves its own tested code path rather than being
  bolted on. `GameType::Null` exists in `src/Trump.h` so protocol/UI code
  can name it, but `SkatGame` rejects any attempt to announce it
  (`actionRejected`), and the announcement UI never offers it.
- **Bot skill level**: any seat without a connected human is played by
  `Bot` (`src/Bot.h`) -- a rules-legal but non-strategic stand-in. It plays
  a uniformly random legal card (it does follow suit/trump correctly, since
  that's a rule, not a strategy), bids only a couple of steps above the
  baseline before giving up, discards its two lowest-point cards, and always
  declares a Suit game in whichever suit it holds the most cards of. It has
  no notion of hand strength, safe leads, or the point count needed to make
  its own bid. Treat it as "fills an empty seat", not "an opponent".

## Card-point values

Same 120-point value table as Schafkopf (Ass=11, 10=10, König=4, Ober=3,
Unter=2, 9/8/7=0) -- a real coincidence in German card-game history shared
by two otherwise-unrelated games, not a bug carried over between this
tutorial's Skat and Schafkopf modules.

## Must-follow-suit

Trump is its own "suit" once a trump card is led: if the led card is trump,
a player holding any trump must play trump; otherwise they must follow the
suit led if they hold a non-trump card of it; if neither applies, any card
may be played. The highest trump played wins a trick outright; if no trump
was played, the highest card of the suit led wins.

## Win condition

The declarer needs 61+ of the 120 points across the tricks they win plus
whatever ends up in the Skat after their discard (the 2 buried cards always
count for the declarer, per standard Skat practice, even though they are no
longer the original 2 face-down cards after pickup/discard). The two
defenders' combined tricks form the opposing total; there is no separate
"defenders need 60" tracking since 120 - 61 = 59 already implies it.

## Architecture (REQ-GAME-02)

- `src/Card.{h,cpp}`, `src/Deck.{h,cpp}`: pure C++23 card/deck representation
  and dealing, no Qt GUI or networking dependency beyond `QString`/Qt
  containers already used repo-wide for card codes.
- `src/Trump.{h,cpp}`: trump-order and trick-winner determination for Suit
  and Grand games (see "Simplifications" for the exact ladder).
- `src/Bidding.h/.cpp`: the simplified round-robin numeric bidding state
  machine.
- `src/Bot.{h,cpp}`: the basic non-strategic bot described above.
- `src/SkatGame.{h,cpp}`: the authoritative rules engine -- dealing,
  bidding, Skat pickup/discard, announcement, trick sequencing, and
  win/lose determination. Driven entirely by `TableMessage` in
  (`applyMessage`) and out (the `send` signal); it never touches
  `QTcpSocket`, `TableServer`, or QML. Only the hosting process runs an
  instance of it.
- `src/SkatController.{h,cpp}`: the only class that talks to
  `games_common_lib`'s networking (`TableServer`/`TableClient`) and to LAN
  discovery (`LanAdvertiser`/`LanBrowser`), and the only class exposed to
  QML (`QML_ELEMENT`). It owns a `SkatGame` when hosting, wiring its `send`
  signal to `TableServer::broadcast`/`sendTo` and `TableServer`'s incoming
  messages to `SkatGame::applyMessage`; when joining, it has no `SkatGame`
  at all and just mirrors whatever the host broadcasts. This is why the
  host's own UI, and every joining client's UI, run through the exact same
  `handleIncomingMessage()` code path.
- `qml/`: hand-painted card faces (`CardView.qml`, `Rectangle`+`Text`, no
  binary art assets) plus the hand/trick/bidding/discard/announcement/result
  panels and the LAN join screen.

## TableMessage protocol (REQ-GAME-04)

All messages use the shared `TableMessage{ type, seat, payload }` envelope
from `industries/games/common`. `seat == -1` on an outgoing message means "broadcast to
every connected seat"; a specific seat means "private to that seat only"
(e.g. a hand of cards, or an error). Card codes are 2 letters: suit
(E/G/H/S) + rank (7/8/9/T/U/O/K/A), e.g. `"HA"` = Herz Ass.

Client -> host:

| type        | payload                                  | meaning                                   |
|-------------|-------------------------------------------|--------------------------------------------|
| `bid`       | `{amount: int}`                          | raise the bid to `amount`                  |
| `pass`      | `{}`                                      | pass for the rest of this auction          |
| `discard`   | `{cards: [code, code]}`                  | declarer buries these 2 cards in the Skat  |
| `announce`  | `{gameType: "suit"\|"grand", trumpSuit}` | declarer's game-type announcement          |
| `playCard`  | `{card: code}`                            | play this card to the current trick        |

Host -> client(s):

| type              | payload                                                          | meaning                                  |
|-------------------|-------------------------------------------------------------------|-------------------------------------------|
| `seatAssigned`    | `{seat: int}` (private)                                          | "you are seat N", sent right after connect|
| `lobby`           | `{dealerSeat, sittingOutSeat, activeSeats: [int], seatIsHuman: [bool]}` | table roster for the upcoming hand |
| `hand`            | `{cards: [code, ...]}` (private)                                 | your current hand                        |
| `biddingTurn`     | `{seat, highestBid, highestBidder}`                              | whose turn it is to bid or pass           |
| `bidAccepted`     | `{seat, amount}`                                                 | that seat's bid was accepted               |
| `passAccepted`    | `{seat}`                                                         | that seat passed                          |
| `biddingFinished` | `{declarerSeat, declarerValue}`                                  | bidding is over                           |
| `skat`            | `{cards: [code, code]}` (private, to declarer)                  | the 2 Skat cards just picked up           |
| `discardAccepted` | `{declarerSeat}`                                                 | the declarer's discard was accepted        |
| `announced`       | `{declarerSeat, gameType, trumpSuit}`                            | the declared game type                    |
| `trickTurn`       | `{seat}`                                                         | whose turn it is to play a card           |
| `cardPlayed`       | `{seat, card}`                                                   | a card was played to the current trick    |
| `trickFinished`   | `{winnerSeat, cards: [{seat, card}], points}`                    | a trick just completed                    |
| `handFinished`    | `{declarerSeat, declarerPoints, defenderPoints, win, skat}`      | the hand is over                          |
| `actionRejected`  | `{reason: string}` (private)                                    | the last action from that seat was illegal|

## Running

Host: launch `skat`, click "Host a new table". Others on the same LAN
segment: launch `skat`, the join screen auto-discovers the host via
`LanBrowser` (`gameId = "skat"`); use the manual host:port fields if UDP
broadcast is blocked on the network. Any seat without a human connected
plays automatically via the basic bot described above.
