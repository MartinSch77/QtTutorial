# Onboard Rider Dashboard (Two-Wheelers)

A kiosk-style Qt Quick digital motorcycle dashboard, representative of software
that would run embedded on the bike's own dash unit: no network, no
persistence, just a real-time, animated HMI driven by a simulated ride cycle.

## Design reference

The overall dash layout — circular speed/rev gauges either side of a centre
stack, a gear-position badge, a lean-angle horizon, a redlined rev counter and
a rider-selectable Rain/Road/Sport/Race power-delivery mode — takes its
**visual language** from the genre of premium sport-bike TFT dashboards and
companion "connected ride" apps (the kind of cluster popularised by bikes like
the Ducati Panigale V4 and connectivity apps like KTM's My Ride). This is
**style/genre inspiration only**: no manufacturer logo, wordmark, colour
palette, font, or exact screen layout is reproduced anywhere in this example.
Every glyph on screen (the motorcycle silhouette, helmet icon, fuel gauge,
gear badge, lean-angle horizon, gauge arcs and needles) is drawn from scratch
with `QtQuick`'s `Canvas` API as plain geometric paths — there are no
manufacturer assets, icon fonts, or downloaded images anywhere in this
example.

## What it demonstrates

- A fully vector, hand-painted UI (`Gauge.qml`, `LeanIndicator.qml`,
  `MotorcycleIcon.qml`, `HelmetIcon.qml`, `FuelGaugeIcon.qml`,
  `GearBadge.qml`) using `QtQuick`'s `Canvas` item for every gauge, icon and
  glyph — no image assets, so it scales to any panel resolution.
- `GearBadge`, a cog-outline badge around the current gear digit, and
  `TyrePanel`, a front/rear tyre-pressure-monitoring readout, both bound live
  to C++ state.
- `LeanIndicator`, the distinctive two-wheeler HMI element: a horizon line
  that rotates by the current lean angle behind a fixed, upright bike
  silhouette, the way a rider's own view of the horizon tilts mid-corner.
- `FuelPanel`, a fuel-gauge readout fed by a simulated tank that drains at a
  rate derived from rpm and the current riding mode (see below) — fuel is not
  independent noise, it tracks how the bike is actually being ridden.
- `RidingModeSelector`, a Rain/Road/Sport/Race power-delivery mode picker:
  selecting a mode changes the dash's accent colour *and* feeds straight into
  `RideCycleSimulator`, visibly changing how eagerly the bike accelerates
  (the "power-delivery curve"), how much cornering lean is let through, and
  how fast the tank drains.
- The rev-counter `Gauge` paints a translucent **redline zone** for the last
  ~1500 rpm of its travel and turns solid red once the needle enters it, the
  way a real tacho marks its danger zone.
- A domain model (`RideCycleSimulator`) that produces a physically plausible,
  correlated ride cycle: speed follows a target profile with first-order lag
  (a motorcycle cannot teleport to a new speed), gear and rpm are both derived
  from that same speed signal, and — the key coupling — **lean angle is a
  function of the cornering phase of the cycle and the current speed within
  it**, not independent noise: it is zero outside the cornering window and
  ramps up and down like a chicane (right, then left) while the bike is
  actually cornering, scaled by how fast it is going through the corner.
  Tyre temperature drifts toward a speed-dependent equilibrium with its own
  thermal lag (sustained speed heats the tyres, the rear more than the
  front), and tyre pressure is a direct function of that temperature, the way
  a real tyre's pressure rises as it warms up. The rider-selected
  `RidingMode` further modulates the speed time constant (Race mode's
  throttle response is snappier than Rain mode's), the fraction of raw
  cornering lean that is let through (Rain caps lean for a cautious ride;
  Race allows the full lean), and the fuel burn rate (higher rpm and higher
  modes burn fuel faster) — none of these are independent of each other or of
  the underlying ride cycle.
- `RiderTelemetry` is a thin `QObject`/`QML_ELEMENT` façade that steps the
  simulator on a `QTimer` and republishes state via `Q_PROPERTY`, plus a
  `Q_INVOKABLE setRidingMode()` the mode-selector buttons call into; all the
  actual logic lives in `RideCycleSimulator`, which has no Qt GUI
  dependencies and is unit tested headlessly.

## Qt modules/APIs exercised

- **Qt Quick / QML** (`QtQuick`, `QtQuick.Window`) for the HMI itself —
  appropriate for embedded dash targets where a GPU-accelerated scene graph
  gives smooth animation at low CPU cost.
- **Qt Quick `Canvas`** for custom-painted gauges and the lean-angle horizon
  instead of chart/image assets (keeps the example royalty-free and
  resolution-independent).
- **`QML_ELEMENT`** / `qt_add_qml_module` to register a C++ type directly for
  QML without manual `qmlRegisterType` boilerplate.
- **`QTimer`** to drive a fixed-step simulation tick, standing in for a real
  IMU/wheel-speed sensor feed.

In a real ISO/IEC/IEEE 12207-governed development, the `RideCycleSimulator`
logic layer is exactly the kind of unit that would sit under a documented
software requirement with traceable unit tests — separating it from the UI is
what makes that traceability possible.

## Build & run

Built as part of the top-level project:

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target two_wheelers_onboard_rider_dashboard
./build/industries/two-wheelers/onboard-rider-dashboard/two_wheelers_onboard_rider_dashboard
```

## Tests

See `tests/industries/two-wheelers/onboard-rider-dashboard/`, which exercises
`RideCycleSimulator`'s speed/gear/rpm correlation, the lean-angle/cornering
coupling, tyre temperature/pressure plausibility, the riding-mode-dependent
power-delivery curve and lean limiting, and the rpm/mode-correlated fuel burn
rate — all without any Qt GUI dependency.
