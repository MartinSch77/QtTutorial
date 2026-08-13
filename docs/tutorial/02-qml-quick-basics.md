# Tutorial: Qt Quick / QML Basics

This walks through `framework-tour/02-qml-quick-basics/` and the QML
concepts it demonstrates: exposing C++ types to QML, list models, and
states/transitions/animations.

## 1. Why QML alongside Widgets?

Module `01-widgets-basics` used Qt Widgets, an immediate, retained-mode,
C++-centric UI toolkit. Qt Quick (QML) is Qt's declarative, scene-graph-based
alternative: UI is described as a tree of items with property bindings, and
it is the toolkit backing Qt Quick Controls, Qt Quick 3D, and Qt Graphs (see
module `09-latest-qt-release-features`). Both toolkits coexist in the same
Qt installation and can be mixed in one application; this module is pure QML
to keep the concepts isolated.

## 2. Exposing a C++ backend to QML

Older Qt code registered C++ types with `qmlRegisterType<T>(...)` in `main.cpp`
or exposed a single object via
`engine.rootContext()->setContextProperty(...)`. Both still work, but the
current recommended approach (Qt 6.2+) is to annotate the class itself:

```cpp
class SystemStatusBackend : public QObject {
    Q_OBJECT
    QML_ELEMENT
    ...
};
```

and let `qt_add_qml_module()` (called from `qttutorial_add_app`, see
`cmake/QtTutorialTargets.cmake`) generate the registration code at build time.
This keeps the registration next to the type instead of in a separate
bootstrap file, and it lets tools like `qmllint` type-check QML against your
C++ API.

`IndicatorListModel` is marked `QML_UNCREATABLE` because it should only ever
be instantiated by `SystemStatusBackend`, not created directly from QML with
`IndicatorListModel {}` — a common pattern for model/backend types that are
handed out via a property rather than constructed in QML.

## 3. `Q_PROPERTY` with `NOTIFY`

```cpp
Q_PROPERTY(QString overallStatus READ overallStatus NOTIFY overallStatusChanged)
```

QML bindings such as `text: backend.overallStatus` re-evaluate automatically
whenever `overallStatusChanged()` is emitted — this is what makes the
uptime counter and overall-status text in `qml/Main.qml` update live without
any polling on the QML side.

## 4. `QAbstractListModel` → `ListView`

`IndicatorListModel::roleNames()` maps integer roles to the property names
(`name`, `value`, `status`) used inside the `ListView`'s `delegate`. Every
second, `SystemStatusBackend::advance()` calls `IndicatorListModel::refresh()`,
which recomputes the readings and emits `dataChanged()` — the `ListView`
picks the change up and only repaints the affected delegates, exactly the
Model/View benefit you get from Qt Widgets' table/list views too (see
module `01-widgets-basics`).

## 5. States, Transitions, and Animations

`qml/IndicatorTile.qml` gives the tile's background `Rectangle` a `state`
bound to the indicator's status string, and declares three `State` objects
(`nominal`, `warning`, `critical`) that each apply `PropertyChanges` to the
border and bar colors. A single `Transition` with a `ColorAnimation` makes
every state change animate smoothly instead of jumping. Independently, a
`SequentialAnimation on scale` pulses the tile while it is in the `critical`
state — showing that imperative-feeling animations (loop, pulse) and
declarative state machines can be combined on the same item.

## 6. Try it yourself

- Add a sixth indicator to `IndicatorGenerator::generate()` in
  `src/IndicatorGenerator.cpp` and watch a new tile appear in the list
  automatically.
- Lower `kWarningThreshold`/`kCriticalThreshold` to see tiles enter the
  warning/critical states sooner.
- Replace the `ColorAnimation` in `IndicatorTile.qml`'s `Transition` with a
  `Behavior` on the `Rectangle`'s `color` property directly, and compare the
  two approaches.
