# Kicker – LAN Table Football (Foosball)

A real-time, LAN-multiplayer table football game for 2-4 players, and the
`games/` category's **3D graphics showcase** (the four card games are all
2D, QML-rendered cards; this game pairs a `QtQuick3D` table with a 2D HUD/
lobby overlay, so between the five games the category demonstrates both 2D
and 3D Qt Quick rendering).

It implements a simplified digital version of table football/foosball as
commonly found in bars and rec rooms, not a specific regional tournament
ruleset (foosball has no single standardized rule set the way, say, Skat
does) — see "Deliberate simplifications" below for exactly how it differs
from a full-size table.

## Deliberate simplifications

- **2 rods per team instead of the usual 3-5.** A real foosball table
  typically has a goalkeeper rod, one or two defense rods, a midfield rod
  and one or two attack rods. This game simplifies to one "defense" rod and
  one "attack" rod per team — 4 rods, 12 figures total — which keeps the
  control scheme (one active rod per player at a time) simple to learn
  while still capturing the core slide-and-spin gameplay.
- **Rotation is a bounded swing, not a free spin.** Real rods can spin
  freely. Here each rod's rotation is clamped to ±180° (`kMaxRotation` in
  `src/KickerTypes.h`) — a "wind up and strike" arc rather than continuous
  spinning. This makes "rotation" a well-defined, boundable quantity for
  both the mouse mapping and the deterministic unit tests.
- **2D collision physics, 3D rendering.** `KickerSimulation` treats the
  ball and figures as points/circles on the table plane (x = slide axis,
  z = goal-to-goal axis) for all physics purposes; the 3D scene renders
  that same state with real depth and shading, but nothing is simulated in
  3D (e.g. the ball never leaves the table surface).
- **No AI opponent.** With fewer than 4 human players, the unassigned rods
  simply hold their position (no input ever arrives for them) — a static,
  uncontrolled rod, not a bot. This is different from the four card games,
  which do simulate AI opponents; a foosball "bot" that convincingly times
  slides and kicks was judged out of scope for this tutorial.
- **No anti-cheat / input validation.** The host trusts whatever rod index
  a client's "input" message names; a client is only ever built to send
  input for the rod(s) it was assigned (see "Network protocol" below).

## Controls

The entire game view is one control surface (not divided into per-rod
click zones):

- **Slide** — the mouse's horizontal position across the window maps
  directly (1:1, clamped) to the active rod's position along the slide
  rail. Move the mouse left/right to walk your figures sideways.
- **Spin (kick)** — the mouse's horizontal *velocity* (how fast the mouse
  is moving left/right, in window-widths per second) maps directly to the
  active rod's angular velocity. A slow, deliberate mouse move only
  nudges the figures' rotation; a fast left-right flick spins them hard
  enough to kick the ball. If the mouse stops moving, the active rod's
  spin decays to zero within ~80ms (see `qml/GameScreen.qml`) so a
  stationary mouse doesn't leave a rod spinning forever.
- **Which rod is "active":**
  - **4-player mode:** each seat controls exactly one specific rod, fixed
    for the whole match — whatever the mouse does always drives that one
    rod.
  - **2-player mode:** each player controls both of their team's rods with
    one mouse. Whichever of the two rods is currently closer to the ball
    (by position along the goal-to-goal axis) is "active" and responds to
    the mouse; the other rod holds its last slide position with zero spin
    until the ball comes back near it. This mirrors how casual digital
    foosball games commonly handle one-mouse-per-team control, and is
    implemented once, in `KickerSimulation::selectActiveRod()`, so the
    input layer and the unit tests agree on the exact same rule.

## Network protocol (REQ-GAME-04)

Kicker is real-time, not turn-based, so its `TableMessage` payloads carry
continuous per-tick state rather than discrete moves — a deliberate
contrast with the four card games, which exchange one message per
game-legal move. One host runs the authoritative simulation; up to 3
clients send input and mirror whatever state the host broadcasts.

Seat numbering: the hosting player is always logical seat 0 (playing
locally, no TCP connection). Remote players connect via `TableClient` and
are assigned `TableServer`'s own 0-based seat numbers, offset by +1 to get
their logical seat (so `TableServer` seat 0's player is logical seat 1,
etc.) — this is entirely internal to `KickerMatch`; nothing on the wire
needs to know about the offset.

Three message types, all built on `games_common_lib`'s
`TableMessage{type, seat, payload}`:

- **`"welcome"`** (host → one newly-connected client, sent once via
  `TableServer::sendTo` right after `seatConnected`): `seat` is the
  client's assigned logical seat. `payload = {"humanSeatCount": <int>}` so
  the client can compute the same seat→rod assignment
  (`KickerSimulation::rodsForSeat`) as the host without it being sent
  explicitly.
- **`"input"`** (client → host, or applied locally without a network hop
  when the host is the sender): `seat` is the sender's logical seat.
  `payload = {"rods": [{"rod": <0-3>, "slideTarget": <double>, "spin":
  <double>}, ...]}`. In 4-player mode this array always has one entry (the
  seat's one fixed rod); in 2-player mode it always has two entries — the
  currently-active rod with live mouse-derived values, and the currently-
  idle rod with its last slide and zero spin, so a rod can never be left
  spinning by a stale message. Sent on every mouse-move event (see
  `qml/GameScreen.qml`'s `MouseArea.onPositionChanged`), plus a periodic
  zero-spin resend while the mouse is idle.
- **`"state"`** (host → all clients, broadcast once per simulation tick,
  50 Hz / `kSimulationTickMs = 20`): `seat = -1`. `payload =
  {"ball": {"x", "z", "vx", "vz"}, "rods": [{"slide", "rotation",
  "angularVelocity"}, ×4], "score": [teamAGoals, teamBGoals]}` — the full
  `MatchState`, encoded by `KickerProtocol::encodeMatchState`. Clients
  never simulate locally; they just display whatever snapshot arrived
  most recently.

LAN discovery reuses `games_common_lib`'s `LanAdvertiser`/`LanBrowser` with
`LanBeacon::gameId = "kicker"`; a manual host-address/port entry field is
also available in the lobby screen for cases where UDP broadcast doesn't
reach across network segments.

## Physics model

All of the physics lives in `KickerSimulation::step(state, dt, inputs)` —
a pure function with no `QObject`, no Qt Quick, no randomness, so the same
`(state, dt, inputs)` always produces the same result. Per tick:

1. Each rod's `slide` is clamped directly to `±kRodSlideRange` from its
   input's `slideTarget` (instant, 1:1 response — real rods respond
   instantly to a push). Each rod's `angularVelocity` is clamped from the
   input's `spin` to `±kMaxAngularSpeed`, and `rotation` is integrated
   (`rotation += angularVelocity * dt`) and clamped to `±kMaxRotation`.
2. **Kicking:** for any rod whose `|angularVelocity|` is at or above
   `kKickAngularThreshold`, each of its 3 figures (at slide offsets
   `-kFigureSpacing, 0, +kFigureSpacing`) is checked against the ball; if
   the ball is within `kKickContactRadius`, the ball's z-velocity gets
   `kKickImpulseScale * angularVelocity` added — the sign of the rod's
   spin determines which way the ball is kicked along the goal-to-goal
   axis.
3. **Ball movement:** friction (`kBallFrictionPerSecond` of speed lost per
   second), a speed cap (`kMaxBallSpeed`), then position integration.
4. **Side walls** (`x = ±kHalfTableWidth`): the ball bounces back with
   `kWallRestitution` (< 1, so it loses some speed on each bounce).
5. **Goal ends** (`z = ±kHalfTableLength`): if the ball's `x` is within
   `±kGoalHalfWidth` of the goal-mouth centre, it passes straight through;
   once its centre fully crosses the goal line, the *other* team's score
   is incremented and the ball resets to the centre with zero velocity.
   Outside the goal mouth, the end acts as a wall exactly like the sides.

## Structure

- `src/KickerTypes.h` — `BallState`, `RodState`, `RodInput`, `MatchState`
  and all the table/rod geometry constants.
- `src/KickerSimulation.{h,cpp}` — the pure physics step, active-rod
  selection, and seat→rod assignment. This is the only file the unit
  tests exercise.
- `src/KickerProtocol.{h,cpp}` — JSON encode/decode for the `"state"` and
  `"input"` payloads described above.
- `src/KickerMatch.{h,cpp}` — the `QObject` that owns the `QTimer`-driven
  authoritative tick (host) or applies received snapshots (client), and
  the `TableServer`/`TableClient`/`LanAdvertiser` wiring. All four of
  these files build into `kicker_lib`.
- `src/KickerController.{h,cpp}` — the QML-facing glue (`QML_ELEMENT`):
  owns a `KickerMatch` and a `LanBrowser`, turns mouse input into rod
  commands, republishes `MatchState` as flat properties. UI-layer only,
  not part of `kicker_lib`, not unit-tested directly.
- `qml/Table3D.qml` — the 3D table: `View3D`, a `PerspectiveCamera`
  angled down over one end, a shadow-casting `DirectionalLight`, and the
  table/ball/rods/figures built entirely from Quick3D's built-in
  primitive meshes (`"#Cube"`, `"#Sphere"`, `"#Cylinder"`) — a stylized,
  low-poly look, not a cop-out for missing assets.
- `qml/Hud.qml` — the 2D score/status/menu overlay drawn on top of the
  3D view.
- `qml/LobbyScreen.qml`, `qml/GameScreen.qml`, `qml/Main.qml` — the lobby
  (host/join/LAN-browse) and in-match screens, and the window that
  switches between them based on `KickerController::connectionStatus`.

## Build & run

From the repository root:

```sh
cmake -S . -B build
cmake --build build --target kicker
./build/games/kicker/kicker
```

## Tests

`tests/games/kicker/test_kicker_simulation.cpp` covers `KickerSimulation`
directly, with no Qt Quick/3D or event loop involved: side-wall reflection
on both sides, goals scored through the goal mouth vs. blocked by the end
wall just outside it (and crediting the correct team either end), a rod's
spin above/below the kick threshold imparting (or not imparting) velocity
in the expected direction, a ball outside kick range being unaffected,
slide/rotation clamping to their rail/rotation bounds for wildly
out-of-range input, `selectActiveRod`'s nearest-to-ball rule, and
`rodsForSeat`'s 2-player vs. 4-player assignment.
