# 10 – Latest Qt Release Features

The flagship "what's new in Qt" showcase, targeting **Qt 6.11.1** (the
current Qt release at the time this module was written): a live, real-time
multi-sensor telemetry dashboard using **Qt Graphs** (`import QtGraphs`) — a
simulated line chart of three telemetry sensors updating several times a
second, plus a live bar chart of their latest readings.

One series (`seriesC`, "Sensor C – Thermal") is deliberately drawn using
`LineSeries.strokeStyle: LineSeries.DashLine` together with `dashPattern`
and `dashOffset` — all three are properties **introduced in Qt 6.11** (along
with `joinStyle` and `lineStyle`, also new in 6.11; `seriesB` sets
`joinStyle: Qt.RoundJoin` to show that one too), confirmed against the Qt
6.11.0 `LineSeries` QML type reference before use. Before this release,
`LineSeries` had no dashing/join/line-style control at all in QML — this is
a genuine, checkable 6.11 addition, not a cumulative "current as of 6.11"
restatement of older Qt Graphs functionality.

**Licensing:** unlike every other module in this repository, this one is
**GPL-3.0-or-later**, not MIT. See [`NOTICE.md`](NOTICE.md) for why — in
short, Qt Graphs is GPLv3-or-commercial only, with no LGPL option.

This module only builds if Qt Graphs is available (Qt >= 6.9, roughly). The
locally installed Qt 6.4.2 used to develop the rest of this repository does
not have it, so `framework-tour/09-latest-qt-release-features/CMakeLists.txt`
guards the whole module behind `find_package(Qt6 COMPONENTS Graphs)` and
`if(TARGET Qt6::Graphs)`, and simply skips it — with a `message(STATUS ...)`
— rather than failing the repository's configure step.

## What it demonstrates

- **Qt Graphs 2D** (`import QtGraphs`): `GraphsView`, `LineSeries` (a live
  scrolling time series per sensor) and `BarSeries`/`BarSet` (a live
  snapshot of the latest readings), themed with `GraphsTheme`.
- A small C++ producer (`TelemetryFeed`, `QML_ELEMENT`) driven by a
  `QTimer`, emitting one `samplesReady(...)` signal per tick that the QML
  side uses to `append()` new points and prune old ones — the same
  "C++ backend + `QML_ELEMENT`" pattern introduced in
  `framework-tour/02-qml-quick-basics`, applied to a streaming-data use case.
- Deterministic, pure sample generation (`TelemetryGenerator`, in
  `graphs_showcase_lib`) kept separate from the timer/QML wiring, and unit
  tested in `tests/framework-tour/09-latest-qt-release-features/`.

## Build & run

Requires Qt >= 6.9 with the Graphs module (Qt Graphs ships as a separate
add-on module; check it's installed via the Qt Maintenance/Online
Installer). Against Qt 6.4.2 this module is silently skipped.

```sh
cmake -S . -B build
cmake --build build --target latest_qt_release_features
./build/framework-tour/09-latest-qt-release-features/latest_qt_release_features
```

## Other genuinely current Qt highlights (Qt 6.9–6.11)

Not implemented here (out of scope for this module), but worth knowing
about if you're evaluating a recent Qt release:

- **Qt GRPC** — a client module (`Qt6::Grpc`, `import QtGrpc` for QML,
  introduced in Qt 6.5 and actively developed through 6.11) for talking to
  gRPC services using Protobuf-generated, Qt-idiomatic client classes
  (`QGrpcCallReply`, `QGrpcClientStream`, `QGrpcServerStream`,
  `QGrpcBidiStream`), covering all four gRPC call shapes (unary, client-,
  server-, and bidirectional-streaming). Useful when a Qt application needs
  to talk to backend services defined by a `.proto` schema instead of a
  hand-rolled REST/JSON API.
- **Qt Quick Controls customization/guideline improvements** — recent
  releases have kept extending how far you can push a single style's visual
  identity via `Customizing Qt Quick Controls`-style overrides (attached
  properties, per-control QML replacements) without abandoning a built-in
  style altogether, plus continued visual refreshes to the `Material` style.
  See `framework-tour/03-quick-controls-styling/README.md` for how styles
  are selected in this repository, and the Qt docs page "Customizing Qt
  Quick Controls" for the underlying mechanism.
- **Qt Safe Renderer** — a separate, safety-oriented product (not part of
  the regular Qt Quick renderer) for rendering safety-critical UI elements
  (e.g. warning lights, gauges) in a way that is independent from — and can
  be certified separately from — the main Qt Quick application rendering
  it, with continued platform/tooling updates in recent releases. This is
  directly relevant to the avionics/space/railway industry showcases
  elsewhere in this repository, where a subset of the UI (safety-critical
  indicators) may need to be rendered through a certified path distinct
  from the rest of the (uncertified) application UI.
