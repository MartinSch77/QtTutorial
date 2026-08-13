# Qt Framework Coverage — What This Repo Actually Exercises

This is an honest audit, not a marketing claim: which Qt modules this
repository genuinely links against and demonstrates somewhere (in
`framework-tour/`, `industries/`, `industries/games/`, or `showcases/`),
and which real, commonly-used parts of Qt it does **not** cover. Generated
by inspecting every `CMakeLists.txt`'s `Qt6::*` link targets and QML
imports across the repo — see the command in the "How this was checked"
section at the bottom to regenerate this list yourself.

## Qt Essentials

| Module | Covered? | Where |
|---|---|---|
| Core | Yes | Everywhere |
| Gui | Yes | Everywhere |
| Widgets | Yes | `framework-tour/01-widgets-basics`, most `offboard-*` industry apps |
| Qml / Quick | Yes | `framework-tour/02-qml-quick-basics`, most `onboard-*` apps, all games, all showcases |
| Quick Controls | Yes | `framework-tour/03-quick-controls-styling`, most Quick-based apps |
| Network | Yes | `framework-tour/04-networking`, `industries/games/common` (LAN transport), several `offboard-*` apps |
| Sql | Yes | `framework-tour/06-sql-persistence`, most `offboard-*` apps' history stores |
| Concurrent | Yes | `framework-tour/05-concurrency-async` (contrasted directly against `std::jthread`) |
| Test | Yes (as infrastructure) | Every `tests/` directory; no dedicated "how to write a Qt Test" *teaching* module exists, though every test in the repo is a working example |
| **Multimedia** | **No** | Not linked anywhere. A real gap — audio/video playback, camera capture — none of this repo's simulated-sensor-feed apps happen to need it, but it's a commonly-used Essentials module with no representative here. |
| **DBus** | **No** | Linux-specific IPC, not covered. |
| **PrintSupport** | **No** | Not covered — no app in this repo produces a printable report/PDF. |
| Xml | Indirect only | Used transitively by Qt itself (e.g. SVG parsing); no module demonstrates `QXmlStreamReader`/`QDomDocument` directly. |

## Qt Add-on modules used here

| Module | Covered? | Where |
|---|---|---|
| SerialPort | Yes | `framework-tour/08-serial-and-devices` (with an honestly-labeled simulated transport) |
| StateMachine | Yes | `framework-tour/07-state-machine`, and reused conceptually in several industry/game state machines |
| Svg | Yes | `showcases/stock-tracker` (an SVG icon rendered via `Qt6::Svg`) |
| Graphs | Yes (guarded) | `framework-tour/09-latest-qt-release-features` (Qt 6.11+ only), `showcases/stock-tracker` (guarded optional) |
| Quick3D | Yes | `industries/games/kicker` (the 3D foosball table), `framework-tour/10-custom-rendering-and-xr` |
| Quick3D Physics | Yes (guarded) | `framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr` |
| Quick3D Xr | Documented only | Same module; `XrMain.qml` exists and is doc-verified real, but is not built/run anywhere (needs XR hardware) |
| WebSockets | Yes (guarded) | `showcases/stock-tracker` (guarded optional; falls back to an in-process feed on older Qt) |
| ShaderTools / private Gui RHI | Yes (guarded) | `framework-tour/10-custom-rendering-and-xr/rhi-under-qml` |
| Qt.labs.lottieqt (Lottie) | Yes | `showcases/stock-tracker` (an order-filled confirmation animation) |

## Qt add-on modules **not** used anywhere in this repo

These are real, actively-used parts of Qt with no representative example
here — listed plainly rather than omitted silently:

- **Qt Positioning / Qt Location** — GPS, maps. Several industry examples
  (agriculture, mining, two-wheelers, railway) simulate position data with
  hand-rolled coordinates rather than `QGeoCoordinate`/`QGeoPositionInfoSource`
  or a real map view — a real, understandable gap given this repo avoids
  bundling map tile assets/API keys, but a gap nonetheless.
- **Qt Bluetooth** — no onboard device-pairing example uses it, even though
  several industry themes (home automation, defence AR HUD) would be
  plausible fits.
- **Qt NFC**, **Qt Sensors** — not used; a mobile-oriented onboard app
  (e.g. `showcases/startrek-bridge` on Android, or a two-wheelers rider
  app) could plausibly use `QSensor`/accelerometer input instead of a
  purely simulated feed, but none currently do.
- **Qt WebEngine / Qt WebView** — no embedded browser anywhere.
- **Qt Remote Objects**, **Qt WebChannel** — no distributed-object or
  web-native-bridge example; `industries/games/common`'s hand-rolled
  JSON-over-TCP protocol was a deliberate choice to keep the LAN transport
  auditable and dependency-light, not a demonstration of Remote Objects.
- **Qt Core5Compat** — not needed; nothing in this repo depends on
  Qt5-only API surface.
- **Internationalization** (`QTranslator`, `.ts`/`.qm` files, `lupdate`/
  `lrelease`) — every UI string in this repo uses `tr()`/`qsTr()` where
  natural, but no module actually ships or loads a translation, and no CI
  step runs `lupdate`/`lrelease`. This is a real, fixable gap for a repo
  explicitly showcasing a *cross-platform* framework.
- **Accessibility** (`QAccessible`) — not addressed anywhere; a legitimate
  gap for a repo aimed partly at "capability showcase for a customer,"
  where accessibility compliance is often a real evaluation criterion.
- **Undo framework** (`QUndoStack`/`QUndoCommand`) — no app demonstrates
  undo/redo, even though `framework-tour/06-sql-persistence`'s task editor
  or the home-automation wall panel would be natural fits.

## Deliberately excluded, not a gap

- **Qt Charts** — GPL-3.0-or-commercial with no LGPL option; superseded by
  Qt Graphs, which this repo uses instead (see `docs/qa/licensing.md`).
- **Qt Virtual Keyboard**, **Qt Interface Framework** — commercial/GPL
  licensing or narrow embedded-automotive scope respectively; out of scope
  for a general framework tour.

## How this was checked

```bash
grep -rhoE "Qt6::[A-Za-z0-9]+" --include="CMakeLists.txt" . | sort -u
```

...cross-referenced by hand against the list of Qt 6 Essentials and Add-on
modules on qt.io. This file should be re-checked (and this command re-run)
whenever a new framework-tour module, industry, game, or showcase is added
that changes which Qt modules this repo actually links.
