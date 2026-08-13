# Onboard Satellite Telemetry & Subsystem Panel

A fullscreen, kiosk-style embedded HMI representative of software that runs
*inside* a spacecraft: orbital parameter readout, a ground-track position
readout, a battery state-of-charge gauge driven by a simulated eclipse cycle,
thermal zone gauges, a row of hand-drawn ops-display icons, and a subsystem
health grid backed by real Qt State Machine instances.

## Design reference

The dark background, monospace-style telemetry readouts, colour-coded
subsystem status board and icon strip take their **visual language** from
the genre of NASA JPL/ESA mission-control and spacecraft telemetry displays -
dark panels, high-contrast readouts, red/amber/green health indicators. This
is a style/genre reference only: no NASA, ESA, JPL, or any other agency's
trademark, logo, wordmark, exact layout, or copyrighted asset is reproduced
anywhere in this app. All icons are procedurally drawn `QPainterPath` vector
shapes (see `IconPainter`), not fetched or adapted from any external image,
icon font, or SVG asset.

## What it demonstrates

- **An internally consistent orbit/power/thermal/ground-track model.**
  `OrbitalSimulator` sweeps true anomaly at a constant rate and derives
  eclipse/sunlit phases from it; `PowerSystem` only charges the battery when
  `solarInputFraction > 0` *and* the panels are sun-pointed, and always
  discharges against a constant bus load; `ThermalSimulator` drifts each of
  four zones toward a solar-input-dependent target with its own thermal time
  constant; `GroundTrackSimulator` derives the sub-satellite latitude/
  longitude from the same true anomaly and a fixed orbital inclination, with
  the Earth rotating underneath it so successive orbits drift westward rather
  than retracing the same ground track, and a pure `isWithinStationView()`
  helper decides whether a (fictitious) fixed ground station is currently in
  view. All four are pure C++23 with no Qt dependency, easy to unit test in
  isolation.
- **A real Qt State Machine.** `SubsystemHealthMachine` wires three `QState`
  objects (Nominal/Caution/Critical) together with `QState::addTransition`,
  driven by the edge-triggered signals of a `SubsystemMonitor` (which
  classifies a parameter against nominal/critical bands and only emits when
  the classification changes, so the machine doesn't re-enter its current
  state on every sample). Four of these machines back the subsystem health
  grid.
- **Hand-painted gauges and icons.** `RadialGauge` is a reusable
  QPainter-drawn radial gauge (no Qt Charts/Graphs — both are GPL/
  commercial-only) used for battery SoC and each thermal zone.
  `IconPainter`/`StatusIconStrip` add a row of procedurally-drawn vector
  icons (`QPainterPath`, no external assets) - a satellite, an orbit arc, a
  ground-station dish, a battery/charge glyph and a warning triangle - so the
  panel reads as a real ops display, not a generic form.
- **An onboard characteristic, by omission.** No `QtNetwork`, no `QtSql`, no
  persistence: everything is generated in-process by a `QTimer`-driven
  `TelemetrySimulator`.

## Qt modules used

- **Qt Widgets** for the fullscreen host window and hand-painted gauges/grid.
- **Qt State Machine** (`Qt6::StateMachine`, `QStateMachine`/`QState`) for the
  subsystem health state machines — a genuine hierarchical-state-machine
  module, not a hand-rolled enum switch.
- **Qt Core** (`QTimer`, `QObject` signals) to drive the simulation loop.

## Process note

Real spacecraft software is typically developed under standards such as the
**ECSS** software engineering standards; this repository does not claim any
such compliance — it is a technology showcase. Where a life-cycle process is
referenced generically elsewhere in this repository, it is **ISO 12207**.

## Build & run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target onboard_satellite_telemetry
./build/industries/space/onboard-satellite-telemetry/onboard_satellite_telemetry
```

Press `Esc` to exit the fullscreen window.

## Tests

`OrbitalSimulator`, `PowerSystem`, `ThermalSimulator`,
`SubsystemHealthMachine` and `GroundTrackSimulator` (including
`isWithinStationView()`) are unit-tested with QTest under
`tests/industries/space/onboard-satellite-telemetry/`.
