# Offboard Plant SCADA

A control-room historian dashboard: the kind of software that runs away from
the plant floor, aggregating readings as if collected from several onboard
units, with real persistence and reporting instead of live-only display.

## Design reference

Visually styled after the genre of control-room SCADA overview screens
popularised by products like **Siemens WinCC** and **Rockwell FactoryTalk
View** — a plant-wide, status-at-a-glance overview of multiple lines/cells,
each drawn as a small colour-coded P&ID-style mimic (tank/valve/conveyor/
motor), alongside a live tag table and a trend/alarm log. This is a
*style/genre reference only*: no Siemens/Rockwell trademark, logo, wordmark,
or exact layout is reproduced. Every mimic glyph is hand-drawn with
`QPainter` paths in `PlantOverviewWidget`, the same approach `TrendWidget`
already uses — no imported icon assets.

## What it demonstrates

- Multiple simulated process points ("tags") shown live in a `QTableView`,
  each with a believable structured signal (periodic component plus slow
  drift, not unstructured noise — `ProcessPointSimulator::valueAt`), now
  grouped into plant lines/cells (`TagDefinition::line`) with plausible
  cross-coupling: a "gated" tag (a flow or conveyor/motor-speed point) falls
  to a small residual whenever its line is in its simulated stopped phase,
  instead of continuing to oscillate as if unaffected equipment kept running.
- A **plant-wide overview** (`PlantOverviewWidget`, backed by the pure,
  independently-testable `summarizeLines()` and its `PlantOverviewModel`
  wrapper): one hand-drawn mimic tile per line, coloured by run/stop state
  and worst alarm severity, so a manager can see the whole plant's health at
  a glance rather than reading a table.
- A hand-drawn `QPainter` trend chart (`TrendWidget`) for whichever tag/line
  is selected — deliberately not Qt Charts / Qt Graphs, both commercial-only;
  this keeps the whole tutorial MIT-licensed while still exercising real
  custom painting.
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
`ProcessPointSimulator` (pure functions of simulated time, including the
per-line run/stop duty cycle in `lineRunningAt()`), `AlarmEvaluator` (pure
severity classification), `Historian` (the SQL layer), `TagTableModel` (a
real `QAbstractTableModel`), and `PlantOverviewModel` (wrapping the pure
`summarizeLines()` aggregation). `MainWindow`, `TrendWidget`, and
`PlantOverviewWidget` — compiled directly into the app target, same pattern
as the widgets-basics reference example — own the `QTimer` that feeds
simulated samples into the historian once a second, repaints the trend for
the selected tag, and refreshes the plant overview.

## Build & run

```
cmake --build . --target offboard_plant_scada
./industries/industrial-automation/offboard-plant-scada/offboard_plant_scada
```

## Tests

```
ctest -R test_plant_scada_logic
```

Covers the signal generator's determinism and bounds (including gated tags
falling to a residual while their line is stopped, and ungated tags
oscillating regardless of line state), the alarm severity thresholds, a full
historian insert/range-query/latest-sample round trip against an in-memory
SQLite database, the tag table model's status column, and the plant
overview's line aggregation (`summarizeLines`) and model wrapper.
