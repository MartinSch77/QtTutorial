# Offboard Pit Operations Dashboard (Mining)

A control-room style pit operations dashboard: software that runs away from
any single truck, aggregating haul-cycle telemetry from many of them, with
persistence and reporting — the "offboard" counterpart to
`onboard-haul-truck-console`.

## What it demonstrates

- `HaulFleetSimulator` stands in for a pit-wide telemetry-ingestion service:
  it produces a deterministic, phase-shifted, physically plausible haul-cycle
  sample per truck (haul state, payload, location placeholder), reusing the
  same four-stage load/haul/dump/return cycle as the onboard console so the
  fleet is domain-consistent, not independent random noise.
- `PitFleetModel`, a `QAbstractTableModel`, is driven by a `QTimer` and
  exposes truck id, current haul state, payload, and location to a
  `QTableView`.
- Cumulative tonnes hauled is a pure function of how many full haul cycles a
  truck has completed (a dump only completes once a full cycle has elapsed),
  so it is exact and reproducible rather than derived from ad hoc counters.
- `HaulHistoryStore` persists every sample to a SQLite database via `QtSql`,
  demonstrating a real back-office pattern: haul-cycle state lands in a table
  (`pit_haul_history`) that can be queried independently of the live model.
  The shipped app uses an in-memory database for a self-contained demo;
  swapping `":memory:"` for a file path in `MainWindow.cpp` persists history
  across runs.
- Selecting a truck in the table queries its recent cumulative-tonnes history
  back out of SQLite and renders it with `ProductionTrendWidget`, a small
  `QPainter`-based line chart (no Qt Charts, which is not MIT licensed).

## Qt modules/APIs exercised

- **Qt Widgets** (`QTableView`, `QSplitter`, model/view) — appropriate for a
  desktop/back-office control room application.
- **QtSql** (`QSqlDatabase`, `QSqlQuery`) for the haul-cycle history archive —
  the kind of durable, queryable storage a pit operations team needs that an
  onboard truck console has no reason to carry.
- **`QPainter`** for a lightweight, dependency-free production trend chart.
- **`QTimer`** simulating a periodic telemetry-ingestion push.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target mining_offboard_pit_operations_dashboard
./build/industries/mining/offboard-pit-operations-dashboard/mining_offboard_pit_operations_dashboard
```

## Tests

See `tests/industries/mining/offboard-pit-operations-dashboard/`, which
covers `HaulFleetSimulator`'s state/payload/cumulative-tonnes logic and
`HaulHistoryStore`'s insert/query behaviour against an in-memory SQLite
database — no UI dependency.
