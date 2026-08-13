# Offboard Farm Operations Dashboard (Agriculture)

A control-room style farm-management dashboard: software that runs away from
any single machine, aggregating pass-progress from many fields/vehicles at
once, with persistence and reporting — the "offboard" counterpart to
`onboard-tractor-console`.

## Design reference

The layout — a farm-wide status overview strip above a detail table, a
field-map/trend tabbed detail pane below — is styled after the *genre* of a
farm-management back-office dashboard such as John Deere's Operations
Center. This is a **style inspiration only**: no John Deere (or any other
vendor's) logo, wordmark, colour scheme, icon set, or exact layout is
reproduced. Every icon and chart here is drawn with plain `QPainter` paths
(see `FarmIconPainter.h`); nothing is copied from, or a clone of, any
trademarked or copyrighted asset.

## What it demonstrates

- `FieldOperationSimulator` stands in for a farm-management telemetry
  service: it produces a deterministic, physically plausible pass-progress
  feed per field (not independent random noise). Fields are dispatched in a
  staggered sequence rather than all starting at once (as a real crew would
  send machines out field by field), each field has its own planned pass
  count standing in for field size, and every dispatched field follows the
  same believable pattern: a short idle/downtime window at the start of each
  pass, coverage advancing steadily while working, short turning windows at
  each row end, and — correlated with that same working/turning/idle state —
  a higher engine load and a faster fuel-burn rate while working than while
  idling or turning. Once a field's planned passes are all done, it reports
  itself `complete` rather than looping forever.
- `FieldOperationsModel`, a `QAbstractTableModel`, is driven by a `QTimer`
  and exposes vehicle id, field id, current pass coverage percent, per-tick
  status ("working"/"idle"/"turning"/"scheduled"/"complete"), and an overall
  field job status/pass count to a `QTableView`.
- **`FieldStatusOverviewWidget`**: a new farm-wide overview strip, painted
  with `QPainter`, showing one tile per field colour-coded by whether that
  field's job is not started / in progress / done — the "which fields still
  need attention today" view a farm manager would want before drilling into
  any single field's detail.
- **`TrendChartWidget`**: a new dependency-free line-chart panel (still
  `QPainter`, matching the map widget's approach; this project deliberately
  avoids Qt Charts, which is not MIT licensed) showing the selected field's
  recent fuel-level history pulled straight from `OperationHistoryStore`.
- `OperationHistoryStore` persists every sample — including engine load and
  fuel level, not just coverage — to a SQLite database via `QtSql`,
  demonstrating a real back-office pattern: history lands in a table
  (`operation_history`) that can be queried independently of the live model,
  which is exactly what feeds the new trend chart. The shipped app uses an
  in-memory database for a self-contained demo; swapping `":memory:"` for a
  file path in `MainWindow.cpp` persists history across runs.
- Selecting a field in the table shows its vehicle position on
  `FieldMapWidget`, a small `QPainter`-based field-boundary map (no Qt Charts,
  which is not MIT licensed) with a hand-drawn tractor-glyph marker
  (`FarmIconPainter.h`) that moves along the boundary as the field's pass
  progresses, alongside a `TrendChartWidget` tab for that same field's fuel
  trend.
- **`FarmIconPainter.h`**: a small set of procedurally-drawn farm-equipment
  glyphs (tractor silhouette, field/crop-row icon, fuel droplet) painted as
  plain geometric `QPainterPath`s — no image/SVG/icon-font assets — shared by
  the map, overview and trend widgets so the dashboard reads as a real
  farm-equipment console rather than a generic form.

## Qt modules/APIs exercised

- **Qt Widgets** (`QTableView`, `QSplitter`, model/view) — appropriate for a
  desktop/back-office operations application.
- **QtSql** (`QSqlDatabase`, `QSqlQuery`) for the operation history archive —
  the kind of durable, queryable storage a farm-management back office needs
  that an in-cab console does not.
- **`QPainter`** for a lightweight, dependency-free field map.
- **`QTimer`** simulating a periodic pass-progress ingestion push.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target agriculture_offboard_farm_operations_dashboard
./build/industries/agriculture/offboard-farm-operations-dashboard/agriculture_offboard_farm_operations_dashboard
```

## Tests

See `tests/industries/agriculture/offboard-farm-operations-dashboard/`, which
covers `FieldOperationSimulator`'s coverage/status/dispatch-staggering logic,
its planned-passes-per-field and job-completion behaviour, its
engine-load/fuel-burn-rate correlation, and `OperationHistoryStore`'s
insert/query behaviour — including the new engine-load/fuel-level history
columns — against an in-memory SQLite database, all with no UI dependency.
