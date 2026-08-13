# Onboard Rider Dashboard (Two-Wheelers)

A kiosk-style Qt Quick digital motorcycle dashboard, representative of software
that would run embedded on the bike's own dash unit: no network, no
persistence, just a real-time, animated HMI driven by a simulated ride cycle.

## What it demonstrates

- A fully vector, hand-painted UI (`Gauge.qml`, `LeanIndicator.qml`) using
  `QtQuick`'s `Canvas` item for the speedometer, rev-counter, and lean-angle
  horizon — no image assets, so it scales to any panel resolution.
- A gear indicator and `TyrePanel`, a front/rear tyre-pressure-monitoring
  readout, both bound live to C++ state.
- `LeanIndicator`, the distinctive two-wheeler HMI element: a horizon line
  that rotates by the current lean angle behind a fixed, upright bike
  silhouette, the way a rider's own view of the horizon tilts mid-corner.
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
  a real tyre's pressure rises as it warms up.
- `RiderTelemetry` is a thin `QObject`/`QML_ELEMENT` façade that steps the
  simulator on a `QTimer` and republishes state via `Q_PROPERTY`; all the
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
coupling, and tyre temperature/pressure plausibility without any Qt GUI
dependency.
