# Offboard Fleet Dashboard (Automotive)

A control-room style fleet management dashboard: software that runs away from
any single vehicle, aggregating telemetry from many of them, with persistence
and reporting — the "offboard" counterpart to `onboard-instrument-cluster`.

## What it demonstrates

- `FleetVehicleSimulator` stands in for a telemetry-ingestion service: it
  produces a deterministic, phase-shifted, physically plausible speed/fuel
  feed per vehicle (not independent random noise), including occasional fault
  codes.
- `FleetModel`, a `QAbstractTableModel`, is driven by a `QTimer` and exposes
  vehicle id, a location placeholder, speed, fuel level, and fault codes to a
  `QTableView`.
- `TelemetryHistoryStore` persists every sample to a SQLite database via
  `QtSql`, demonstrating a real back-office pattern: telemetry lands in a
  table (`telemetry_history`) that can be queried independently of the live
  model. The shipped app uses an in-memory database for a self-contained
  demo; swapping `":memory:"` for a file path in `MainWindow.cpp` persists
  history across runs.
- Selecting a vehicle in the table queries its recent speed history back out
  of SQLite and renders it with `TrendWidget`, a small `QPainter`-based line
  chart (no Qt Charts, which is not MIT licensed).

## Qt modules/APIs exercised

- **Qt Widgets** (`QTableView`, `QSplitter`, model/view) — appropriate for a
  desktop/back-office control room application.
- **QtSql** (`QSqlDatabase`, `QSqlQuery`) for the telemetry history archive —
  the kind of durable, queryable storage an offboard system needs that an
  embedded onboard HMI does not.
- **`QPainter`** for a lightweight, dependency-free trend chart.
- **`QTimer`** simulating a periodic telemetry-ingestion push.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target offboard_fleet_dashboard
./build/industries/automotive/offboard-fleet-dashboard/offboard_fleet_dashboard
```

## Tests

See `tests/industries/automotive/offboard-fleet-dashboard/`, which covers
`FleetVehicleSimulator`'s speed/fuel/fault-code logic and
`TelemetryHistoryStore`'s insert/query behaviour against an in-memory SQLite
database — no UI dependency.
