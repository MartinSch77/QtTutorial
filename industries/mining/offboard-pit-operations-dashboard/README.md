# Offboard Pit Operations Dashboard (Mining)

A control-room style pit operations dashboard: software that runs away from
any single truck, aggregating haul-cycle telemetry from many of them, with
persistence and reporting — the "offboard" counterpart to
`onboard-haul-truck-console`.

## What it demonstrates

- `HaulFleetSimulator` stands in for a pit-wide telemetry-ingestion service:
  it produces a deterministic, phase-shifted, physically plausible haul-cycle
  sample per truck (haul state, payload, speed, fuel burn, a normalised pit
  position, and a brief loading-overfill flag), reusing the same four-stage
  load/haul/dump/return cycle as the onboard console so the fleet is
  domain-consistent, not independent random noise. Speed follows a distinct,
  plausible profile per phase; fuel burn is derived from *both* the current
  payload and current speed (a loaded truck climbing the haul road burns more
  than an empty one coasting back); and each truck's pit-plan position is
  interpolated along a named shovel-to-dump haul-road leg for its current
  phase, so the pit-map view shows continuous motion.
- `PitFleetModel`, a `QAbstractTableModel`, is driven by a `QTimer` and
  exposes truck id, current haul state, payload, speed, fuel burn, and
  location to a `QTableView`, with a hand-painted warning-triangle icon
  (`Qt::DecorationRole`, drawn by `PitIcons::warningTriangleIcon`) flagging any
  truck currently in its brief loading-overfill window.
- `PitMapWidget` is a new `QPainter`-based top-down pit-overview panel: a
  generic open-pit motif (concentric benches, drawn by `PitIcons`) with each
  truck rendered as a small haul-truck glyph at its live simulated position,
  coloured by haul-cycle phase, with the same warning glyph overlaid on an
  overloaded truck.
- A second `ProductionTrendWidget` now renders a fleet-wide productivity trend
  (total tonnes hauled across the whole fleet over time), alongside the
  original per-truck trend that appears when a row is selected — reusing the
  same dependency-free `QPainter` line-chart widget rather than introducing a
  second charting approach.
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
- **`QPainter`** for the lightweight, dependency-free production trend chart,
  the pit-map view, and the hand-painted warning/truck glyphs in `PitIcons`
  (no icon font or image assets). `Qt6::Gui` is linked explicitly into the
  static library for these — it is already an implicit dependency of this
  app's `Qt6::Widgets`, not a new Qt module being introduced.
- **`QTimer`** simulating a periodic telemetry-ingestion push.

## Design reference

The fleet table, warning glyphs and pit-overview map take **style
inspiration only** from the genre of pit-wide fleet-management dashboards
popularised by systems such as Caterpillar MineStar and Komatsu FrontRunner
— a control-room view of truck state, payload and position across a whole
fleet. No Caterpillar or Komatsu trademark, logo, wordmark, colour system or
exact screen layout is reproduced anywhere in this example; the pit map and
all icons here are original `QPainter` drawings created for this tutorial.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target mining_offboard_pit_operations_dashboard
./build/industries/mining/offboard-pit-operations-dashboard/mining_offboard_pit_operations_dashboard
```

## Tests

See `tests/industries/mining/offboard-pit-operations-dashboard/`, which
covers `HaulFleetSimulator`'s state/payload/cumulative-tonnes logic, its
loading-overfill overload flag, its payload/speed-correlated fuel-burn model,
and its pit-position interpolation, plus `HaulHistoryStore`'s insert/query
behaviour against an in-memory SQLite database — no UI dependency.
