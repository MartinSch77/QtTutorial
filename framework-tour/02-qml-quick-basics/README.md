# 02 – Qt Quick / QML Basics

A small "live system status" dashboard: a handful of animated indicator tiles
(CPU load, memory, network I/O, disk I/O, temperature) that update once a
second and change color when they cross warning/critical thresholds.

## What it demonstrates

- **Exposing C++ to QML with `QML_ELEMENT`.** `SystemStatusBackend` and
  `IndicatorListModel` (`src/SystemStatusBackend.h`, `src/IndicatorListModel.h`)
  are registered via `QML_ELEMENT` and `qt_add_qml_module` rather than the
  older `qmlRegisterType`/context-property approach — the modern, CMake-driven
  way to build a QML module introduced in Qt 6.2 and used throughout Qt 6.
- **`Q_PROPERTY` with `NOTIFY`.** `SystemStatusBackend::overallStatus` and
  `uptimeSeconds` are read-only properties that emit change signals every
  tick, driving live text bindings in `qml/Main.qml`.
- **`QAbstractListModel` feeding a `ListView`.** `IndicatorListModel` adapts
  the pure data source (`IndicatorGenerator`, in the `qml_basics_lib` static
  library) to the roles (`name`, `value`, `status`) that `ListView`'s
  `delegate` binds against.
- **States, Transitions and Animation in QML.** `qml/IndicatorTile.qml` uses a
  `states`/`transitions` declaration to switch its border/bar color between
  nominal/warning/critical, a `Behavior on width` for the live value bar, and
  a `SequentialAnimation on scale` that pulses tiles in the critical state.

## Structure

- `src/IndicatorGenerator.{h,cpp}` (in `qml_basics_lib`) — pure, deterministic,
  unit-tested data generation. No `QObject`, no randomness: the same tick
  always produces the same readings, which is what makes it easy to test.
- `src/IndicatorListModel.{h,cpp}` — the `QAbstractListModel` wrapper.
- `src/SystemStatusBackend.{h,cpp}` — the QTimer-driven backend exposed as the
  QML root context type.
- `qml/Main.qml`, `qml/IndicatorTile.qml` — the UI.

## Build & run

From the repository root:

```sh
cmake -S . -B build
cmake --build build --target qml_quick_basics
./build/framework-tour/02-qml-quick-basics/qml_quick_basics
```

## Tests

`tests/framework-tour/02-qml-quick-basics/test_indicator_generator.cpp` covers
`IndicatorGenerator` directly (determinism, value range, threshold
classification) without needing a QML engine or event loop.
