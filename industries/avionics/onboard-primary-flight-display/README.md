# Onboard Primary Flight Display

A fullscreen, kiosk-style embedded HMI representative of software that runs
*inside* the aircraft: a classic Primary Flight Display with an attitude
indicator (artificial horizon), airspeed and altitude tapes, a heading
indicator, a round rotating compass rose, a vertical-speed indicator, and a
caution/warning annunciator panel.

## Design reference

The instrument genre here - artificial horizon, vertical airspeed/altitude
tapes, a rotating compass card, a vertical-speed needle, and a small
annunciator strip - is a style reference to the general look and feel of
modern glass-cockpit primary flight displays such as the **Garmin
G3000/G5000** and **Honeywell Primus Epic** families. This is inspiration for
the *genre* of instrument, not a clone: no Garmin/Honeywell logo, wordmark, or
exact copyrighted layout/colour scheme is reproduced anywhere in this code.
Every glyph (the aircraft silhouette, the caution triangle, the compass rose)
is drawn from scratch as plain geometric `QPainterPath`/`QPainter` primitives -
no external icon font, image, or SVG asset is used.

## What it demonstrates

- **Custom QPainter instrumentation.** The attitude indicator is painted from
  scratch with real trigonometry — the horizon plane is rotated by roll and
  translated by pitch using `QPainter` transforms (`rotate`/`translate`), not a
  static image. The airspeed/altitude tapes, heading card, and VSI are all
  hand-painted as well.
- **A coupled flight-dynamics model.** `FlightDynamics` (in `onboard_pfd_lib`)
  is a small, pure C++23 class with no Qt dependency at all: control inputs
  (elevator/aileron/throttle) drive damped first-order responses in pitch and
  roll, which in turn drive heading rate (bank → turn), vertical speed
  (pitch × airspeed), airspeed (climbing bleeds speed), and now engine
  temperature (throttle demand plus a climb-power penalty). Every instrument
  therefore moves consistently with the others, the way a demo audience
  expects a real PFD to behave, and "the engine is running hot" is a genuine
  consequence of a sustained high-power climb rather than an independent
  random flag.
- **A caution/warning annunciator, driven by that same coupled state.**
  `AnnunciatorLogic` (also pure C++23, independently unit-tested) evaluates
  the current `FlightState` against a small set of avionics-style thresholds -
  excessive bank angle, overspeed, low-energy/approach-to-stall airspeed, a
  high sink rate close to the ground, and an over-temperature engine - and the
  `AnnunciatorPanel` widget lights up a "MASTER CAUTION" lamp plus per-condition
  legends with a procedurally-drawn caution-triangle glyph whenever any of
  them fire.
- **A round compass rose alongside the linear heading tape.** `CompassRose` is
  a second, independent heading presentation: the full 360° card rotates
  behind a fixed lubber line and a small top-down aircraft silhouette (both
  drawn as plain geometric shapes), the classic rotating-compass-card
  convention, complementing rather than duplicating the existing
  `HeadingIndicator` tape.
- **An onboard characteristic, by omission.** There is no `QtNetwork`, no
  `QtSql`, and no persistence anywhere in this app — everything needed to run
  is generated in-process by a `QTimer`-driven simulator
  (`FlightDataSimulator`). That is the point: embedded flight-deck software is
  a closed, self-contained real-time system.

## Qt modules used

- **Qt Widgets** for the fullscreen host window and all custom-painted
  instrument widgets (`QWidget::paintEvent` + `QPainter`).
- **Qt Core** (`QTimer`, `QObject` signals) to drive the simulation loop at a
  fixed tick rate without blocking the UI thread.

## Process note

Real avionics software of this kind is typically developed and certified
under **DO-178C**; this repository does not claim any such compliance — it is
a technology showcase, not a certified artifact. Where a life-cycle process is
referenced generically elsewhere in this repository, it is **ISO 12207**.

## Build & run

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target onboard_primary_flight_display
./build/industries/avionics/onboard-primary-flight-display/onboard_primary_flight_display
```

Press `Esc` to exit the fullscreen window.

## Tests

`FlightDynamics` and `AnnunciatorLogic` are unit-tested with QTest under
`tests/industries/avionics/onboard-primary-flight-display/` — see
`test_flight_dynamics.cpp` (build target: `test_flight_dynamics`) and
`test_annunciator_logic.cpp` (build target: `test_annunciator_logic`).
