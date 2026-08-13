# Onboard Primary Flight Display

A fullscreen, kiosk-style embedded HMI representative of software that runs
*inside* the aircraft: a classic Primary Flight Display with an attitude
indicator (artificial horizon), airspeed and altitude tapes, a heading
indicator, and a vertical-speed indicator.

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
  (pitch × airspeed), and airspeed (climbing bleeds speed). Every instrument
  therefore moves consistently with the others, the way a demo audience
  expects a real PFD to behave.
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

`FlightDynamics` is unit-tested with QTest under
`tests/industries/avionics/onboard-primary-flight-display/` — see
`test_flight_dynamics.cpp`. Build target: `test_flight_dynamics`.
