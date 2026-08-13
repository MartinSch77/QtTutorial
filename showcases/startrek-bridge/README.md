# Starship Bridge (LCARS-style)

A general-purpose, visually striking showcase: a starship-bridge dashboard
in the "LCARS" visual language popularized by a well-known sci-fi
franchise - swept elbow-shaped panels, a bold sans-serif font, and an
orange/gold + lavender/purple + blue-on-black color scheme - built entirely
from hand-painted Qt Quick, not from any copyrighted assets.

A single window switches between a main viewscreen and three bridge
stations (Helm/Navigation, Engineering, Tactical/Science), all bound to one
shared simulated ship state so, for example, calling Red Alert from the
status strip visibly changes the color language and readings across every
station at once, not just the viewscreen.

## Original-visual-language note (read this first)

This showcase deliberately recreates the *shape and color language* of an
LCARS-style interface from scratch - the elbow bracket shape is hand-drawn
with `Canvas` path/arc calls in `LcarsElbowFrame.qml`, gauges are hand-drawn
radial dials (`RadialGauge.qml`), and the starfield is procedurally
generated points, not an image. No copyrighted logos, fonts, textures, or
other binary art assets from any real production are bundled or referenced.
In-universe flavour text is kept generic and original: station names
("Helm", "Engineering", "Tactical/Science"), the placeholder ship name ("USS
Horizon"), sensor-contact labels ("Unknown vessel", "Debris field", ...) and
deck names ("Bridge", "Medical Bay", "Cargo Bay", ...) are all invented for
this demo, not specific character names, ship names, or exact dialogue from
any copyrighted work. The simulated "stardate" readout uses an invented
formula (year and day-of-year folded into a steadily increasing decimal
number) rather than reproducing any specific in-universe stardate formula.

## What it demonstrates

- **A pure, testable simulation core shared by every station.**
  `ShipState` (in `startrek_bridge_lib`) owns the ship-wide `AlertLevel`
  plus three independent sub-simulators - `NavigationSimulator` (heading,
  fictional warp/impulse speed, nearby local-space contacts),
  `EngineeringSimulator` (warp-core power output, hull integrity, internal
  temperature) and `TacticalSimulator` (external radiation, hull stress, a
  cosmetic shield-strength percentage, external sensor contacts) - all
  plain C++23 with no Qt Quick dependency, driven forward by one
  `advance(dtSeconds)` call per tick using the *same* `AlertLevel`, so
  raising Red Alert visibly changes engineering power draw, tactical shield
  strength and deck life-support margins together, consistently.
- **A real Qt State Machine per deck.** `DeckLifeSupportMonitor` classifies
  a deck's composite life-support reading into Nominal/Caution/Critical and
  emits a signal only when the classification changes;
  `DeckLifeSupportMachine` wires those edge-triggered signals into a real
  `QStateMachine` (three `QState`s) via `QState::addTransition`. Six of
  these machines back the deck life-support grid on the Engineering
  station, so status changes read as believable transitions, not random
  per-frame flicker.
- **Hand-painted, not charted.** `RadialGauge.qml` (power/hull/temp/shield
  dials), the local-space plot on the Helm station, and the animated
  starfield on the viewscreen are all drawn with QML `Canvas` - no Qt
  Charts/Graphs, both of which are GPL/commercial-only and unsuitable for
  this MIT-licensed tutorial.
- **One QML-facing facade over the whole simulation.** `BridgeSimulator`
  (`QML_ELEMENT`) owns a `ShipState` and a `QTimer`, republishing it as
  QML-bindable properties (`Q_PROPERTY`s and `QVariantList`s), the same
  "thin QObject facade over a pure C++ model" pattern used throughout this
  repository's other industry examples.
- **A responsive layout for "all the screens."** `Main.qml`'s
  `wideLayout` property (`width >= 820`) switches the station selector
  between a left sidebar (desktop-ish aspect ratios) and a bottom tab strip
  (phone-ish aspect ratios) purely via anchor bindings - resize the window
  to see it reflow live.

## Qt modules used

- **Qt Quick / QML** for the entire UI, all hand-painted (`Canvas`, plain
  `Item`/`Rectangle`/`ListView`/animations) - no image assets.
- **Qt State Machine** (`Qt6::StateMachine`, `QStateMachine`/`QState`) for
  the per-deck life-support state machines.
- **Qt Core** (`QTimer`, `QObject` signals/properties) to drive the
  simulation loop and expose it to QML.

## Build & run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target startrek_bridge
./build/showcases/startrek-bridge/startrek_bridge
```

## Android deployment

This is exactly the kind of touch-friendly, resolution-independent QML app
Qt for Android is meant for. To actually run it on a phone/tablet, build it
with the Android NDK toolchain via `qt-cmake` (Qt's CMake wrapper configured
for an Android Qt kit) or via Qt Creator's Android kit, then deploy the
resulting APK to a device or emulator - see Qt's
["Building Qt for Android" / deployment
guide](https://doc.qt.io/qt-6/android.html) for the toolchain setup.
**This repository's own CI (`.github/workflows/ci.yml`) builds Linux,
Windows and macOS only** - it does not provision the Android NDK/SDK, so
Android deployment is documented here but not CI-verified, the same
"documented but not exercised everywhere" honesty standard used by
`framework-tour/09-latest-qt-release-features`'s Qt Graphs guard.

## Tests

`tests/showcases/startrek-bridge/` covers, with QTest:

- `test_ship_state.cpp` - alert-level transitions and that setting Red Alert
  is reflected consistently across the shared engineering/tactical state.
- `test_deck_life_support_machine.cpp` - the deck life-support
  `QStateMachine`'s Nominal/Caution/Critical transitions, including the
  `QStateMachine::start()` gotcha (it enters its initial state
  asynchronously, so tests give the event loop a turn with
  `QTest::qWait`/`QTRY_COMPARE` before driving values through it).
- `test_navigation_simulator.cpp` / `test_tactical_simulator.cpp` - the
  simulated navigation/sensor-contact data staying within plausible bounds
  (heading normalized, warp/impulse/shield percentages clamped, contacts
  staying inside their simulated area) over many thousands of simulated
  ticks.
