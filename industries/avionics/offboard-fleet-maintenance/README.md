# Offboard Fleet Maintenance Dashboard

An operations/back-office dashboard, the "offboard" counterpart to the onboard
Primary Flight Display: it aggregates simulated telemetry from a small fleet
of aircraft as if it had arrived from a ground data link, surfaces
predictive-maintenance flags, tracks a maintenance task list, ranks the whole
fleet by how soon each aircraft is due for its next scheduled inspection, and
persists parameter history to SQLite.

## Design reference

The visual language here - a dense, table/report-oriented operations board
with status colour-coding and a "big board" fleet overview - is a style
reference to the general genre of glass-cockpit-adjacent ground operations
tooling such as **Garmin G3000/G5000**-integrated flight-data-services
dashboards and **Honeywell Primus Epic**-class ground support tools. This is
inspiration for the genre, not a clone: no Garmin/Honeywell logo, wordmark, or
exact copyrighted layout is reproduced anywhere in this code. The
caution-triangle glyph in the maintenance-due overview is drawn from scratch
as a plain geometric `QPainterPath`, not an external icon asset.

## What it demonstrates

- **A real predictive-maintenance heuristic.** `EngineTrend` (pure C++23, no
  Qt) keeps a rolling window of a parameter (here, engine vibration) and flags
  it when it is out of its nominal band *or* when a least-squares trend slope
  projects it out of band soon — not random jitter, an actual (if simplified)
  drift-detection algorithm.
- **A believable fleet simulation.** `FleetSimulator` walks each aircraft's
  engine parameters with bounded random noise pulling back toward a nominal
  value, and seeds two aircraft with a slow directional drift fault so the
  detector has something genuine to catch over time. It also accumulates each
  aircraft's flight hours and cycles since its last scheduled inspection (at a
  demo-accelerated rate).
- **A real correlation between an engine anomaly and maintenance urgency.**
  `InspectionScheduler` (pure C++23, no Qt, independently unit-tested) computes
  hours/cycles remaining to the next scheduled inspection and classifies each
  aircraft as Nominal, Due Soon, or Urgent - and an aircraft that is merely due
  soon is escalated to Urgent specifically when it *also* has an active
  engine-parameter anomaly from `EngineTrend`, the same way a real operations
  desk would prioritize an aircraft that is both due for a look and already
  showing a symptom, rather than treating utilization and health as unrelated
  signals.
- **A fleet-wide maintenance-due overview.** `MaintenanceOverviewWidget` ranks
  every aircraft by that urgency (most urgent first) with a bar showing hours
  remaining plus a cycles-remaining count, and a procedurally-drawn caution
  triangle on anything urgent - a "big board" summary a non-technical manager
  can read in seconds, complementing the existing per-aircraft detail table.
- **A selectable trend chart.** The existing hand-painted `TrendWidget` now
  drives from a parameter selector (vibration, EGT margin, or oil pressure),
  reusing the same QPainter-based charting approach for whichever parameter is
  selected instead of being hard-wired to one signal.
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
`FlightHistoryStore` is exercised against an in-memory SQLite database in
`test_flight_history_store`, `InspectionScheduler`'s urgency classification
(including the due-soon-plus-anomaly escalation) is covered by
`test_inspection_scheduler`, and the end-to-end correlation inside
`FleetSimulator` — a seeded drift fault eventually escalating a due-soon
aircraft to an urgent maintenance task — is covered by
`test_fleet_inspection_correlation`.
