# Offboard Fleet Dashboard (Two-Wheelers)

A control-room style dashboard for a bike-share/delivery fleet of e-bikes and
e-scooters: software that runs away from any single vehicle, aggregating
telemetry from many of them, with persistence and reporting — the "offboard"
counterpart to `onboard-rider-dashboard`.

## Design reference

The two-wheelers *onboard* rider dashboard in this repo (see
`../onboard-rider-dashboard/README.md`) takes visual inspiration from the
genre of premium sport-bike TFT dashboards and connected-ride apps (e.g.
Ducati Panigale-style clusters, KTM's My Ride app) — **style/genre reference
only, no manufacturer assets, logos, or exact layouts reproduced**. This
offboard fleet dashboard is a different kind of software (a back-office
control room, not a rider-facing HMI), so it borrows the *icon language*
rather than the layout: a procedurally-drawn motorcycle silhouette identifies
vehicle rows, a cog/service badge (the fleet-ops equivalent of a
gear-position badge) flags maintenance-due vehicles, and a small analogue
needle gauge (the fleet equivalent of a fuel gauge, reused here for battery
level) sits next to the battery reading. All icons are drawn with
`QPainterPath`, matching this app's existing `TrendWidget` convention of
hand-painted `QPainter` graphics rather than image/SVG assets.

## What it demonstrates

- `FleetVehicleSimulator` stands in for a telemetry-ingestion service: it
  produces a deterministic, phase-shifted, physically plausible speed/battery
  feed per vehicle (not independent random noise), including periodic
  charging stops (speed drops to zero, status flips to "charging" once battery
  runs low) and occasional maintenance windows. It also derives a per-vehicle
  odometer (`odometerKmAt`, accruing distance at a deterministic per-vehicle
  usage rate) and a `maintenanceDue` flag that goes true for the last
  stretch of km before each service-interval multiple — a mileage-based
  "service due soon" signal, deliberately independent of the short-lived
  "currently being serviced" `Maintenance` status.
- `FleetModel`, a `QAbstractTableModel`, is driven by a `QTimer` and exposes
  vehicle id, a location placeholder, speed, battery level, rider status
  ("riding" / "idle" / "charging" / "maintenance"), odometer and
  maintenance-due flag to a `QTableView`. `FleetIconDelegate` paints the
  motorcycle/battery-gauge/service-badge icons from `FleetIcons.h` alongside
  the id, battery and service columns.
- `FleetAnalytics` computes fleet-wide utilization (the percentage of
  vehicles currently `Riding`) as a pure function over a snapshot of
  samples, and `FleetUtilizationHistory` keeps a bounded rolling history of
  that percentage so the dashboard can plot a **fleet-wide utilization
  trend**, rendered with a second `TrendWidget` alongside the per-vehicle
  speed trend.
- `TelemetryHistoryStore` persists every sample to a SQLite database via
  `QtSql`, demonstrating a real back-office pattern: telemetry lands in a
  table (`two_wheelers_fleet_history`) that can be queried independently of
  the live model. The shipped app uses an in-memory database for a
  self-contained demo; swapping `":memory:"` for a file path in
  `MainWindow.cpp` persists history across runs.
- Selecting a vehicle in the table queries its recent speed history back out
  of SQLite and renders it with `TrendWidget`, a small `QPainter`-based line
  chart (no Qt Charts, which is not MIT licensed).

## Qt modules/APIs exercised

- **Qt Widgets** (`QTableView`, `QSplitter`, model/view) — appropriate for a
  desktop/back-office control room application.
- **QtSql** (`QSqlDatabase`, `QSqlQuery`) for the telemetry history archive —
  the kind of durable, queryable storage an offboard system needs that an
  onboard HMI has no reason to carry.
- **`QPainter`** for a lightweight, dependency-free trend chart.
- **`QTimer`** simulating a periodic telemetry-ingestion push.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target two_wheelers_offboard_fleet_dashboard
./build/industries/two-wheelers/offboard-fleet-dashboard/two_wheelers_offboard_fleet_dashboard
```

## Tests

See `tests/industries/two-wheelers/offboard-fleet-dashboard/`, which covers
`FleetVehicleSimulator`'s speed/battery/status/odometer/maintenance-due logic,
`FleetAnalytics`'s utilization aggregation and bounded history, and
`TelemetryHistoryStore`'s insert/query behaviour against an in-memory SQLite
database — no UI dependency.
