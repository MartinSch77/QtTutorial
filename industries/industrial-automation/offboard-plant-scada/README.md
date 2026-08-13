# Offboard Plant SCADA

A control-room historian dashboard: the kind of software that runs away from
the plant floor, aggregating readings as if collected from several onboard
units, with real persistence and reporting instead of live-only display.

## What it demonstrates

- Multiple simulated process points ("tags") shown live in a `QTableView`,
  each with a believable structured signal (periodic component plus slow
  drift, not unstructured noise — `ProcessPointSimulator::valueAt`).
- A hand-drawn `QPainter` trend chart (`TrendWidget`) — deliberately not Qt
  Charts / Qt Graphs, both commercial-only; this keeps the whole tutorial
  MIT-licensed while still exercising real custom painting.
- An **actual** SQLite-backed historian (`Historian`) via `QtSql`: a real
  schema (`tag_id`, `timestamp`, `value`), parameterized `INSERT`/`SELECT`
  statements, and range queries so the trend can be pulled back out for any
  time window rather than kept only in memory.
- An alarm history log driven by a pure severity classifier (`AlarmEvaluator`).

## Qt modules exercised

- **Qt SQL** (`Qt6::Sql`, `QSQLITE` driver) — the historian.
- **Qt Widgets** — table/trend/log dashboard layout.
- **Qt Core** — the model/view classes and the simulated signal generator.

## Architecture

`plant_scada_lib` holds everything genuinely testable without a GUI:
`ProcessPointSimulator` (pure functions of simulated time), `AlarmEvaluator`
(pure severity classification), `Historian` (the SQL layer), and
`TagTableModel` (a real `QAbstractTableModel`). `MainWindow` and `TrendWidget`
— compiled directly into the app target, same pattern as the widgets-basics
reference example — own the `QTimer` that feeds simulated samples into the
historian once a second and repaints the trend for the selected tag.

## Build & run

```
cmake --build . --target offboard_plant_scada
./industries/industrial-automation/offboard-plant-scada/offboard_plant_scada
```

## Tests

```
ctest -R test_plant_scada_logic
```

Covers the signal generator's determinism and bounds, the alarm severity
thresholds, a full historian insert/range-query/latest-sample round trip
against an in-memory SQLite database, and the tag table model's status
column.
