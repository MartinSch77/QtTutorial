# Offboard Farm Operations Dashboard (Agriculture)

A control-room style farm-management dashboard: software that runs away from
any single machine, aggregating pass-progress from many fields/vehicles at
once, with persistence and reporting — the "offboard" counterpart to
`onboard-tractor-console`.

## What it demonstrates

- `FieldOperationSimulator` stands in for a farm-management telemetry
  service: it produces a deterministic, phase-shifted, physically plausible
  pass-progress feed per field (not independent random noise), including a
  short idle/downtime window at the start of each pass and short turning
  windows at each row end.
- `FieldOperationsModel`, a `QAbstractTableModel`, is driven by a `QTimer`
  and exposes vehicle id, field id, current pass coverage percent, and status
  ("working"/"idle"/"turning") to a `QTableView`.
- `OperationHistoryStore` persists every sample to a SQLite database via
  `QtSql`, demonstrating a real back-office pattern: coverage progress lands
  in a table (`operation_history`) that can be queried independently of the
  live model. The shipped app uses an in-memory database for a self-contained
  demo; swapping `":memory:"` for a file path in `MainWindow.cpp` persists
  history across runs.
- Selecting a field in the table shows its vehicle position on
  `FieldMapWidget`, a small `QPainter`-based field-boundary map (no Qt Charts,
  which is not MIT licensed) with a marker that moves along the boundary as
  the field's pass progresses.

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
covers `FieldOperationSimulator`'s coverage/status/phase-shift logic and
`OperationHistoryStore`'s insert/query behaviour against an in-memory SQLite
database — no UI dependency.
