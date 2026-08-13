# Tutorial: Qt Widgets Basics

This walks through `framework-tour/01-widgets-basics/` — a small unit
converter — and the concepts every later module builds on: the Model/View
split, and keeping domain logic independent of both Qt and the UI.

## 1. The domain layer has no Qt dependency

`src/UnitConversion.h`/`.cpp` is plain C++23: a `Unit` struct, a static
catalogue of units grouped by physical quantity, and a `convert()` function
returning `std::expected<double, ConversionError>` rather than throwing or
returning a sentinel value. It doesn't include a single Qt header. This is
deliberate — it's the same idea `docs/qa/traceability.md`'s "non-UI logic in
a separate library" requirement is checking for, just at its smallest scale:
a pure function is trivially unit-testable, portable, and reusable from a
Widgets UI, a QML UI, or a command-line tool without change.

```cpp
[[nodiscard]] std::expected<double, ConversionError> convert(
    double value, std::string_view fromSymbol, std::string_view toSymbol);
```

`tests/framework-tour/01-widgets-basics/test_unit_conversion.cpp` tests this
directly — no `QApplication`, no widget, just the conversion function.

## 2. The Model/View split

`UnitConverterModel` is a `QAbstractTableModel`: each row is one conversion
request (value, from-unit, to-unit), and the fourth column is *computed* on
every `data()` call by calling the pure `convert()` function above. Editing
a cell (`setData()`) just updates the row's stored fields and emits
`dataChanged()` — the result column recomputes itself because `QTableView`
re-queries `data()` for the whole row.

This is the core Qt Model/View idea: the model owns and validates data, the
view (`QTableView` here) only renders whatever the model reports, and
`Qt::ItemFlags`/roles (`Qt::DisplayRole`, `Qt::EditRole`) control what's
editable. Later modules use the same idea for a `QAbstractListModel`
(`02-qml-quick-basics`), `QSqlTableModel` (`06-sql-persistence`), and several
industry examples' fleet/patient/satellite tables.

## 3. Wiring it into a window

`MainWindow` owns the model and a `QTableView`, plus a toolbar with
"Add row" / "Remove selected" buttons connected via lambda slots — the
standard modern (Qt 5.4+) way to connect a signal to arbitrary code without
declaring a named slot for something this small.

## 4. Build and run

```bash
cmake --build build --target widgets_basics
./build/framework-tour/01-widgets-basics/widgets_basics
```

## Next

`docs/tutorial/02-qml-quick-basics.md` — the same "separate the logic,
expose a small typed surface to the UI" idea, but with Qt Quick/QML instead
of Widgets.
