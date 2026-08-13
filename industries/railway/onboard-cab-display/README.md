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

Covers `RouteProfile` lookups (permitted speed, next restriction, next
station), the braking-curve function's boundary and monotonicity behaviour,
and the train simulator's gradual acceleration/deceleration and station
dwell/door cycle.
