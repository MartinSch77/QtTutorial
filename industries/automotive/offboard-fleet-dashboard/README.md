# Offboard Fleet Dashboard (Automotive)

A control-room style fleet management dashboard: software that runs away from
any single vehicle, aggregating telemetry from many of them, with persistence
and reporting — the "offboard" counterpart to `onboard-instrument-cluster`.

## Design reference

The dashboard's emphasis on a fleet-wide efficiency trend, a per-vehicle
efficiency score, and a maintenance-due telltale takes *style* inspiration
from the same genre of glanceable, ambient-accented digital instrumentation
popularised by products like Mercedes-Benz's MBUX and Tesla's Model 3
cluster — reinterpreted here for a back-office control room rather than the
vehicle itself. This is a genre/style reference only: no Mercedes-Benz or
Tesla logo, wordmark, trademark, or exact layout is reproduced, and the
maintenance/status icons are hand-drawn `QPainter` vector paths rather than
any imported icon asset.

## What it demonstrates

- `FleetVehicleSimulator` stands in for a telemetry-ingestion service: it
  produces a deterministic, phase-shifted, physically plausible speed/fuel
  feed per vehicle (not independent random noise), including occasional fault
  codes, a driving-efficiency score derived from how far the vehicle's speed
  strays from the fleet's ideal cruising speed, and an odometer (the exact
  closed-form integral of that same speed signal) that drives a periodic,
  wraparound "maintenance due" window — the same single-source-of-truth
  correlation convention the speed/fuel model already uses.
- `FleetModel`, a `QAbstractTableModel`, is driven by a `QTimer` and exposes
  vehicle id, a location placeholder, speed, fuel level, efficiency,
  maintenance status, and fault codes to a `QTableView`. The maintenance
  column is painted by `FleetMaintenanceDelegate`, a small `QStyledItemDelegate`
  that draws a vector wrench icon (due soon) or check-mark badge (OK) with
  `QPainter` paths instead of plain text — this app has no QML/Canvas
  available, so hand-painted `QPainter` vector shapes are the Widgets
  equivalent of the onboard app's Canvas-painted icons.
- `TelemetryHistoryStore` persists every sample to a SQLite database via
  `QtSql`, demonstrating a real back-office pattern: telemetry lands in a
  table (`telemetry_history`) that can be queried independently of the live
  model. The shipped app uses an in-memory database for a self-contained
  demo; swapping `":memory:"` for a file path in `MainWindow.cpp` persists
  history across runs.
- Selecting a vehicle in the table queries its recent speed history back out
  of SQLite and renders it with `TrendWidget`, a small `QPainter`-based line
  chart (no Qt Charts, which is not MIT licensed). A second `TrendWidget`
  instance renders a fleet-wide efficiency trend — the rolling average of
  every vehicle's efficiency score, refreshed every tick — reusing the exact
  same charting approach rather than introducing a second dependency.

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
`FleetVehicleSimulator`'s speed/fuel/fault-code logic, its
efficiency-vs-cruising-speed curve, and its odometer/maintenance-due
wraparound behaviour, plus `TelemetryHistoryStore`'s insert/query behaviour
against an in-memory SQLite database — no UI dependency.
