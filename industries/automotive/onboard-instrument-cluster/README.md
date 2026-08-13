# Onboard Instrument Cluster (Automotive)

A kiosk-style Qt Quick digital instrument cluster, representative of software that
would run embedded on the vehicle itself: no network, no persistence, just a
real-time, animated HMI driven by a simulated drivetrain.

## Design reference

The visual language — a large glanceable speed gauge flanked by secondary
readouts, an ambient accent colour that shifts with the active driving mode,
and a dedicated efficiency readout — takes *style* inspiration from the genre
of modern digital instrument clusters/"hyperscreens" popularised by products
like Mercedes-Benz's MBUX and Tesla's Model 3 cluster. This is a genre/style
reference only: no Mercedes-Benz or Tesla logo, wordmark, trademark, or exact
layout is reproduced anywhere in this example, and no external image or icon
assets are used at all — every glyph on screen is drawn as vector geometry
(`Canvas`/`Rectangle`/`Text`) in QML.

## What it demonstrates

- A fully vector, hand-painted UI using `QtQuick`'s `Canvas` item and simple
  geometric shapes for every instrument: the speedometer/rev-counter arcs
  (`Gauge.qml`), a P/R/N/D gear-selector rung (`GearSelector.qml`), chevron
  turn-signal arrows (`TurnSignalArrow.qml`), a fuel-pump pictogram with a
  level bar (`FuelGauge.qml`), and a four-wheel tire-pressure-monitoring
  mini-display (`TirePressurePanel.qml`) — no image assets anywhere, so the
  cluster scales to any panel resolution.
- A gear indicator, blinking turn signals, and a simplified ADAS
  following-distance strip, all bound live to C++ state.
- A driving-mode selector (`DrivingModeSelector.qml`, Eco/Comfort/Sport) that
  re-tints the cluster's ambient accent colour and visibly changes the
  simulated efficiency readout and fuel-burn rate — Sport mode chases the
  target speed more aggressively and burns more fuel, Eco mode responds more
  gently and sips it, and hard acceleration costs efficiency regardless of
  mode.
- A domain model (`DriveCycleSimulator`) that produces a physically plausible,
  correlated drive cycle: speed follows a target profile with first-order lag
  (a car cannot teleport to a new speed) whose time constant is set by the
  active driving mode, gear and rpm are both derived from that same speed
  signal (so upshifts/downshifts and rpm sawtooth are consistent with speed),
  fuel drains with distance travelled at a mode-dependent rate, tire pressure
  drifts down per-wheel at a different deterministic rate per tire (wrapping
  around once it triggers a low-pressure warning, the same convention the fuel
  model uses so the demo reaches the interesting state quickly), and a
  following-distance warning is tied to the braking phase of the cycle — none
  of it independent random noise.
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
`DriveCycleSimulator`'s speed/gear/rpm correlation, fuel depletion,
following-distance logic, driving-mode-dependent efficiency/fuel-burn curves,
and per-wheel tire-pressure drift/warning behaviour — all without any Qt GUI
dependency.
