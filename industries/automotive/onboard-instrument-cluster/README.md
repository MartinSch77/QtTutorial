# Onboard Instrument Cluster (Automotive)

A kiosk-style Qt Quick digital instrument cluster, representative of software that
would run embedded on the vehicle itself: no network, no persistence, just a
real-time, animated HMI driven by a simulated drivetrain.

## What it demonstrates

- A fully vector, hand-painted UI (`Gauge.qml`, `AdasStrip.qml`) using
  `QtQuick`'s `Canvas` item for the speedometer and rev-counter arcs — no image
  assets, so it scales to any panel resolution.
- A gear indicator, blinking turn signals, and a simplified ADAS
  following-distance strip, all bound live to C++ state.
- A domain model (`DriveCycleSimulator`) that produces a physically plausible,
  correlated drive cycle: speed follows a target profile with first-order lag
  (a car cannot teleport to a new speed), gear and rpm are both derived from
  that same speed signal (so upshifts/downshifts and rpm sawtooth are
  consistent with speed), fuel drains with distance travelled, and a
  following-distance warning is tied to the braking phase of the cycle — not
  independent random noise.
- `VehicleTelemetry` is a thin `QObject`/`QML_ELEMENT` façade that steps the
  simulator on a `QTimer` and republishes state via `Q_PROPERTY`; all the
  actual logic lives in `DriveCycleSimulator`, which has no Qt GUI
  dependencies and is unit tested headlessly.

## Qt modules/APIs exercised

- **Qt Quick / QML** (`QtQuick`, `QtQuick.Window`) for the HMI itself —
  appropriate for embedded targets where a GPU-accelerated scene graph gives
  smooth animation at low CPU cost, important for automotive-grade silicon.
- **Qt Quick `Canvas`** for custom-painted gauges instead of chart/image
  assets (keeps the example royalty-free and resolution-independent).
- **`QML_ELEMENT`** / `qt_add_qml_module` to register a C++ type directly for
  QML without manual `qmlRegisterType` boilerplate.
- **`QTimer`** to drive a fixed-step simulation tick, standing in for a
  real sensor/CAN bus feed.

In a real ISO 26262 / ASPICE development, the `DriveCycleSimulator` logic
layer is exactly the kind of unit that would sit under a documented software
requirement with traceable unit tests — separating it from the UI is what
makes that traceability possible.

## Build & run

Built as part of the top-level project:

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target onboard_instrument_cluster
./build/industries/automotive/onboard-instrument-cluster/onboard_instrument_cluster
```

## Tests

See `tests/industries/automotive/onboard-instrument-cluster/`, which exercises
`DriveCycleSimulator`'s speed/gear/rpm correlation, fuel depletion, and
following-distance logic without any Qt GUI dependency.
