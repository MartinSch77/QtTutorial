# Onboard Haul Truck Console (Mining)

A kiosk-style Qt Quick operator console, representative of software that
would run embedded in the cab of a large mining haul truck: no network, no
persistence, just a real-time, animated HMI driven by a simulated haul cycle.

## What it demonstrates

- A fully vector, hand-painted UI (`Gauge.qml`, `HaulStateStrip.qml`,
  `TyrePressureGrid.qml`) using `QtQuick`'s `Canvas` item for the payload and
  temperature gauges — no image assets, so it scales to any panel resolution.
- A prominent four-segment haul-cycle banner (Loading / Hauling / Dumping /
  Returning) and a six-tyre pressure grid, all bound live to C++ state.
- A domain model (`HaulCycleSimulator`) that drives an explicit small state
  machine over the four haul-cycle phases with plausible dwell times (loading
  30 s, hauling 90 s, dumping 15 s, returning 60 s), and derives every other
  signal from that same state/time so they stay correlated the way a real
  truck's systems are: payload ramps up during loading (briefly overfilling
  before the load is trimmed back to rated capacity — the overload warning
  reflects that trim window), stays at rated capacity through hauling, ramps
  back to zero during dumping, and sits at zero while returning empty; engine
  temperature climbs while loading and hauling a full load and eases off while
  returning empty; retarder temperature climbs specifically while descending
  the haul road into the dump point (modelled as the last portion of hauling)
  and through dumping, then cools on the way back; and tyre pressure creeps up
  slightly with payload rather than varying independently.
- `HaulTruckTelemetry` is a thin `QObject`/`QML_ELEMENT` façade that steps the
  simulator on a `QTimer` and republishes state via `Q_PROPERTY`; all the
  actual logic lives in `HaulCycleSimulator`, which has no Qt GUI dependencies
  and is unit tested headlessly.

## Qt modules/APIs exercised

- **Qt Quick / QML** (`QtQuick`, `QtQuick.Window`) for the HMI itself —
  appropriate for embedded targets where a GPU-accelerated scene graph gives
  smooth animation at low CPU cost on constrained cab-mounted hardware.
- **Qt Quick `Canvas`** for custom-painted gauges instead of chart/image
  assets (keeps the example royalty-free and resolution-independent).
- **`QML_ELEMENT`** / `qt_add_qml_module` to register a C++ type directly for
  QML without manual `qmlRegisterType` boilerplate.
- **`QTimer`** to drive a fixed-step simulation tick, standing in for a real
  onboard sensor/CAN bus feed.

In a real ISO/IEC/IEEE 12207-governed development, the `HaulCycleSimulator`
logic layer is exactly the kind of unit that would sit under a documented
software requirement with traceable unit tests — separating it from the UI is
what makes that traceability possible.

## Build & run

Built as part of the top-level project:

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target mining_onboard_haul_truck_console
./build/industries/mining/onboard-haul-truck-console/mining_onboard_haul_truck_console
```

## Tests

See `tests/industries/mining/onboard-haul-truck-console/`, which exercises
`HaulCycleSimulator`'s haul-cycle ordering, payload/overload behaviour, and
engine/retarder temperature response without any Qt GUI dependency.
