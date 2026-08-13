# Onboard Tractor Console (Agriculture)

A kiosk-style Qt Quick in-cab console for a tractor or combine, representative
of software that would run embedded on the machine itself: no network, no
persistence, just a real-time, animated HMI driven by a simulated field pass.

## Design reference

The layout and gauge set are styled after the *genre* of a modern in-cab
precision-ag terminal — a top status strip, a field-coverage/guidance map as
the centrepiece, and a ring of round gauges for fuel/engine load/yield —
broadly the same visual language as John Deere's Gen 4 in-cab display /
Operations Center. This is a **style inspiration only**: no John Deere (or
any other vendor's) logo, wordmark, colour scheme, icon set, or exact layout
is reproduced. Every icon and gauge here is an original, procedurally-drawn
vector shape (see "Icons" below); nothing is copied from, or a clone of, any
trademarked or copyrighted asset.

## What it demonstrates

- A fully vector, hand-painted UI (`Gauge.qml`, `CrossTrackBar.qml`,
  `CoverageArc.qml`, `FieldCoverageMap.qml`, `FarmIcon.qml`) using
  `QtQuick`'s `Canvas` item for the fuel/engine-load/yield-rate dials, the
  GPS-guided steering-assist scale, the pass-progress ring, the field
  coverage map, and a small set of farm-equipment glyphs — no image or SVG
  assets, so it scales to any panel resolution.
- **Icons** (`FarmIcon.qml`): a tractor silhouette, a field/crop-row icon, a
  seed/leaf icon, plow and planter implement icons, and a fuel-droplet icon,
  each drawn as simple geometric vector paths on a `Canvas` (lines, arcs,
  triangles, bezier curves) rather than fetched from any icon font or image
  asset.
- **`FieldCoverageMap.qml`**: a top-down field-coverage map. The field is
  laid out as `fieldRowCount` horizontal strips driven boustrophedon-style
  (forward along one row, turn, back along the next); rows already driven
  are painted as a solid covered swath, the current row fills from the entry
  side as the tractor progresses through it, and a marker tracks the
  tractor's live position — giving the pass-progress numbers in the header a
  literal, spatial home instead of just a percentage.
- An implement status readout (engaged/raised, working depth, direction of
  travel) with a matching implement icon, a yield-rate gauge, and the
  original field-coverage arc (now read as the current-pass progress ring),
  all bound live to C++ state.
- A domain model (`FieldPassSimulator`) that produces a physically plausible,
  correlated field pass: coverage advances deterministically with distance
  travelled along a fixed-length pass (not randomly), cross-track error is a
  smooth, bounded function of total distance travelled (a sum of two
  sinusoids of different periods, standing in for steering-assist correction
  dynamics rather than independent per-tick noise), the implement disengages
  in short turn zones at each row end, engine load and fuel-burn rate are
  both continuous functions of that same engaged state (fuel burn rate scales
  with engine load, not just a fixed "working vs idle" constant), and yield
  rate is zero while the implement is raised and scales linearly with ground
  speed while it is engaged. Row index/heading is derived from the pass
  number so the field-coverage map can lay passes out boustrophedon-style.
  When a pass completes, the simulator wraps into a new pass and increments
  the pass counter rather than stopping.
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
determinism, engaged-vs-disengaged engine load/fuel logic, the fuel-burn-rate
vs. engine-load correlation, the yield-rate vs. speed/engagement correlation,
and the row-index/heading boustrophedon logic behind the field-coverage map
— all without any Qt GUI dependency.
