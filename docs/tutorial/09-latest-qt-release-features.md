# Tutorial: Latest Qt Release Features (Qt Graphs)

This walks through `framework-tour/09-latest-qt-release-features/`: a live
multi-sensor telemetry dashboard built on **Qt Graphs**, targeting **Qt
6.11.1** — including `LineSeries` dash/join/line-style properties that are
new *in* 6.11, not just "supported as of" it (see §5) — plus a survey of a
few other genuinely recent Qt highlights.

## 1. Licensing comes first here

Before anything else: read [`../../framework-tour/09-latest-qt-release-features/NOTICE.md`](../../framework-tour/09-latest-qt-release-features/NOTICE.md).
This is the one module in the repository licensed GPL-3.0-or-later instead
of MIT, because Qt Graphs is GPLv3-or-commercial with no LGPL escape hatch —
the same constraint that historically applied to Qt Charts. If you're
building a proprietary application and don't have a commercial Qt license,
this is the deciding factor in whether you can use Qt Graphs at all.

## 2. Why this module might not build for you

`CMakeLists.txt` does:

```cmake
find_package(Qt6 COMPONENTS Graphs QUIET)
if(NOT TARGET Qt6::Graphs)
    message(STATUS "Qt6::Graphs not found ... - skipping ...")
    return()
endif()
```

Qt Graphs is a separate, newer add-on module (needs roughly Qt >= 6.9) that
is not part of a default Qt 6.4 installation. Rather than making the whole
repository's CMake configure fail on an older Qt, the module just doesn't
build — the rest of the framework tour and industry showcases are
unaffected. This is the standard pattern for optional-module graceful
degradation in a multi-module CMake project.

## 3. The Qt Graphs QML API

```qml
import QtGraphs

GraphsView {
    axisX: ValueAxis { ... }
    axisY: ValueAxis { ... }
    LineSeries { name: "Sensor A" }
}
```

`GraphsView` is the container: it owns exactly one X and one Y axis and
renders whichever series types (`LineSeries`, `BarSeries`, `ScatterSeries`,
...) are declared as its children — `seriesList` is `GraphsView`'s default
property, so series added as QML children are collected automatically, no
explicit `addSeries()` call needed for static series declared in QML.

`LineSeries`/`ScatterSeries`/`SplineSeries` share a common `XYSeries` base
with `append(x, y)`, `at(index)`, `remove(index)`, and `replace(...)` — this
module's `Connections { onSamplesReady: ... }` handler in `qml/Main.qml`
uses exactly these to append a new point per tick and prune points older
than a fixed history window, giving the classic "scrolling live chart"
effect.

`BarSeries` instead holds one or more `BarSet`s, each an array of values
mapped onto `BarCategoryAxis` categories; the same handler calls
`BarSet.replace(index, value)` each tick to keep a small live bar chart of
each sensor's latest reading.

## 4. The C++ producer

`TelemetryFeed` (`src/TelemetryFeed.h`) is a `QML_ELEMENT`-registered
`QObject` holding a `QTimer`. Every tick it advances an elapsed-time clock
and emits:

```cpp
signals:
    void samplesReady(double elapsedSeconds, double sensorA, double sensorB, double sensorC);
```

QML's `Connections { target: feed; function onSamplesReady(...) }` receives
this. The actual sample values come from `TelemetryGenerator::valueAt(...)`
(`src/TelemetryGenerator.h`, in the `graphs_showcase_lib` static library) —
a pure function of sensor index and elapsed time, with no dependency on the
timer or QML at all, which is what makes it directly unit-testable in
`tests/framework-tour/09-latest-qt-release-features/test_telemetry_generator.cpp`.

## 5. A feature that is specifically new in Qt 6.11

`LineSeries.strokeStyle`, `dashPattern`, `dashOffset`, `joinStyle`, and
`lineStyle` did not exist before Qt 6.11 — before this release `LineSeries`
had no dash/join/line-style control in QML at all. `qml/Main.qml` uses this
for real, not just in prose:

```qml
LineSeries {
    id: seriesB
    name: qsTr("Sensor B – Vibration")
    joinStyle: Qt.RoundJoin          // new in 6.11
}
LineSeries {
    id: seriesC
    name: qsTr("Sensor C – Thermal (dashed via Qt 6.11 strokeStyle)")
    strokeStyle: LineSeries.DashLine // new in 6.11
    dashPattern: [6, 3]              // new in 6.11
    dashOffset: 0                    // new in 6.11
}
```

This is the difference this module is trying to draw between "current as of
Qt 6.11" (Qt Graphs itself, which has existed since 6.6/6.9 depending on the
sub-module) and "introduced *in* Qt 6.11" (this specific set of `LineSeries`
properties) — both are worth knowing when evaluating a Qt upgrade, but only
the latter is a genuinely new capability you couldn't reach for on 6.10.

## 6. Other Qt 6.9–6.11 highlights worth knowing

See the module's `README.md` for a short, doc-verified summary of Qt GRPC,
continued Qt Quick Controls customization improvements, and Qt Safe
Renderer — none implemented in this module, but relevant context for anyone
evaluating a Qt upgrade, and (for Qt Safe Renderer specifically) directly
relevant to this repository's avionics/space/railway industry showcases.

## 7. Try it yourself

- Change `historySeconds` in `qml/Main.qml` to make the scrolling window
  longer or shorter.
- Add a fourth sensor: extend `kSensorSpecs` in
  `src/TelemetryGenerator.cpp`, add a matching parameter to
  `TelemetryFeed::samplesReady`, and add a fourth `LineSeries`/`BarSet`
  entry in `qml/Main.qml`.
- Swap `GraphsTheme.ColorScheme.Dark` for `.Light` and see how the whole
  chart re-themes without touching series or axis code.
