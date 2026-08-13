# Onboard Cab Display

A fullscreen, kiosk-style Qt Quick driver's cab display of the kind that
would be embedded in the driving desk itself: no network, no persistence,
just live, physically consistent train state.

## What it demonstrates

- An ETCS-like speed supervision presentation: current speed, permitted
  speed, and a driving-advice margin bar showing distance and target speed
  for the next restriction, all derived from one `RouteProfile` (a sequence
  of speed-restriction segments) rather than picked independently.
- A real braking-curve function (`maxSafeSpeedKmh`, in `BrakingCurve.h`):
  `v = sqrt(target² + 2·a·d)`, the same shape used by real ETCS speed
  supervision, so the permitted speed shrinks smoothly as the train
  approaches a slower segment or a station instead of snapping.
- A `TrainSimulator` that integrates position and speed (with bounded
  acceleration/deceleration) rather than teleporting the train, including a
  dwell-time door cycle at each station.
- Signal aspect and door-status indicators driven by the same simulated
  state as the speed display.
- A circular speed dial (`SpeedDial.qml`) with a needle for current speed and
  a target-speed marker for the permitted speed, green/red zone split at the
  supervision limit — a DMI-style analogue readout alongside the digital one.
- A hysteresis-based brake-warning latch (`BrakingCurve::nextBrakeWarningState`):
  it activates once speed exceeds the permitted speed by an "on" margin and
  only clears once speed has dropped back to a (smaller) "off" margin, so it
  doesn't flicker right at the threshold the way real ATP brake-intervention
  indications are debounced. It is wired to a pulsing "OVER SPEED — BRAKE"
  banner in the UI.
- A DMI-style "planning area" strip (`PlanningStrip.qml`, backed by
  `RouteProfile::upcomingRestrictions`) showing every upcoming speed
  restriction within a lookahead window, not just the single next one.
- Procedurally-drawn vector icons — a train-front silhouette, a three-lamp
  signal-aspect head, a track/rail glyph, a brake/warning triangle, and doors
  that visually part when open — all drawn as geometric paths on QML
  `Canvas`, not imported from any icon font or image asset.

## Design reference

The layout and iconography are a style/genre reference to Alstom/Siemens-style
ETCS Driver Machine Interfaces (circular speed dial with a target-speed
needle, a signal-aspect lamp, a distance-to-target readout, a "planning area"
strip of upcoming restrictions) — the kind of cab display used across modern
European rolling stock. This is inspiration only: no trademark, logo,
wordmark, exact colour palette, or precise layout of any specific vendor's
product is reproduced, and every icon is drawn procedurally rather than
copied from a real product's asset set.

## Qt modules exercised

- **Qt Quick / QML** — the cab display itself.
- **Qt Core** — `QTimer`-driven simulation loop, `Q_PROPERTY` bindings into QML.

## Architecture

`onboard_cab_display_lib` holds `RouteProfile`, `BrakingCurve`, and
`TrainSimulator` — all of it testable without a QML engine. `main.cpp`
builds a demo route (three speed segments and two stations) and exposes the
single `TrainSimulator` instance to QML as the `train` context property.

## Build & run

```
cmake --build . --target onboard_cab_display
./industries/railway/onboard-cab-display/onboard_cab_display
```

## Tests

```
ctest -R test_cab_display_logic
```

Covers `RouteProfile` lookups (permitted speed, next restriction, upcoming
restrictions within a lookahead window, next station), the braking-curve
function's boundary and monotonicity behaviour, the brake-warning hysteresis
latch, and the train simulator's gradual acceleration/deceleration, station
dwell/door cycle, transient-overspeed brake-warning behaviour, and the
QVariantList exposed for the planning strip.
