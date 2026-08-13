# Onboard Tractor Console (Agriculture)

A kiosk-style Qt Quick in-cab console for a tractor or combine, representative
of software that would run embedded on the machine itself: no network, no
persistence, just a real-time, animated HMI driven by a simulated field pass.

## What it demonstrates

- A fully vector, hand-painted UI (`Gauge.qml`, `CrossTrackBar.qml`,
  `CoverageArc.qml`) using `QtQuick`'s `Canvas` item for the fuel/engine-load
  dials, the GPS-guided steering-assist scale, and the field-coverage arc — no
  image assets, so it scales to any panel resolution.
- An implement status readout (engaged/raised, working depth) and a
  field-coverage arc with the current pass number, all bound live to C++
  state.
- A domain model (`FieldPassSimulator`) that produces a physically plausible,
  correlated field pass: coverage advances deterministically with distance
  travelled along a fixed-length pass (not randomly), cross-track error is a
  smooth, bounded function of total distance travelled (a sum of two
  sinusoids of different periods, standing in for steering-assist correction
  dynamics rather than independent per-tick noise), the implement disengages
  in short turn zones at each row end, and engine load/fuel burn are both
  derived from that same engaged state — higher load and faster fuel burn
  while working than while turning. When a pass completes, the simulator
  wraps into a new pass and increments the pass counter rather than stopping.
- `TractorTelemetry` is a thin `QObject`/`QML_ELEMENT` façade that steps the
  simulator on a `QTimer` and republishes state via `Q_PROPERTY`; all the
  actual logic lives in `FieldPassSimulator`, which has no Qt GUI
  dependencies and is unit tested headlessly.

## Qt modules/APIs exercised

- **Qt Quick / QML** (`QtQuick`, `QtQuick.Window`) for the HMI itself —
  appropriate for in-cab display units where a GPU-accelerated scene graph
  gives smooth animation at low CPU cost.
- **Qt Quick `Canvas`** for custom-painted gauges instead of chart/image
  assets (keeps the example royalty-free and resolution-independent).
- **`QML_ELEMENT`** / `qt_add_qml_module` to register a C++ type directly for
  QML without manual `qmlRegisterType` boilerplate.
- **`QTimer`** to drive a fixed-step simulation tick, standing in for a
  real GPS/CAN implement-control feed.

In a real ISO/IEC/IEEE 12207-governed development, the `FieldPassSimulator`
logic layer is exactly the kind of unit that would sit under a documented
software requirement with traceable unit tests — separating it from the UI is
what makes that traceability possible.

## Build & run

Built as part of the top-level project:

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target agriculture_onboard_tractor_console
./build/industries/agriculture/onboard-tractor-console/agriculture_onboard_tractor_console
```

## Tests

See `tests/industries/agriculture/onboard-tractor-console/`, which exercises
`FieldPassSimulator`'s coverage/wraparound, cross-track-error bounds and
determinism, and engaged-vs-disengaged engine load/fuel logic without any Qt
GUI dependency.
