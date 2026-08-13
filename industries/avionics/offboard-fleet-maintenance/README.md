# Offboard Fleet Maintenance Dashboard

An operations/back-office dashboard, the "offboard" counterpart to the onboard
Primary Flight Display: it aggregates simulated telemetry from a small fleet
of aircraft as if it had arrived from a ground data link, surfaces
predictive-maintenance flags, tracks a maintenance task list, and persists
parameter history to SQLite.

## What it demonstrates

- **A real predictive-maintenance heuristic.** `EngineTrend` (pure C++23, no
  Qt) keeps a rolling window of a parameter (here, engine vibration) and flags
  it when it is out of its nominal band *or* when a least-squares trend slope
  projects it out of band soon — not random jitter, an actual (if simplified)
  drift-detection algorithm.
- **A believable fleet simulation.** `FleetSimulator` walks each aircraft's
  engine parameters with bounded random noise pulling back toward a nominal
  value, and seeds two aircraft with a slow directional drift fault so the
  detector has something genuine to catch over time.
- **SQLite-backed history via QtSql.** `FlightHistoryStore` opens a SQLite
  database, creates its schema, and both writes and reads through *parameterized*
  `QSqlQuery::prepare`/`bindValue` statements.
- **The offboard characteristic.** Unlike the onboard PFD, this app persists
  data, is table/report oriented, and is the kind of system that would run in
  a control room or cloud back end aggregating data from many aircraft, not
  embedded in any single one.

## Qt modules used

- **Qt Widgets** for the dashboard: `QTableView` + `QAbstractTableModel` for
  the fleet table, `QListWidget` for maintenance tasks, and a hand-painted
  `TrendWidget` (QPainter, not Qt Charts — which is GPL/commercial-only) for
  the selected aircraft's vibration trend.
- **Qt Sql** (`QSqlDatabase`, `QSqlQuery`, SQLite driver) for durable
  parameter history.
- **Qt Core** (`QTimer`) to drive the periodic simulation/persistence tick.

## Process note

Real airline maintenance software typically operates under a life-cycle
process such as **ISO 12207**; this repository is a technology showcase and
does not claim any certification or compliance status.

## Build & run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target offboard_fleet_maintenance
./build/industries/avionics/offboard-fleet-maintenance/offboard_fleet_maintenance
```

## Tests

`EngineTrend` is unit-tested with QTest under
`tests/industries/avionics/offboard-fleet-maintenance/` (`test_engine_trend`),
and `FlightHistoryStore` is exercised against an in-memory SQLite database in
`test_flight_history_store`.
