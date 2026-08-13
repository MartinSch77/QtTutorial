# Offboard Fleet Operations Centre

A rail operations dashboard: the kind of software that runs in a control
room or on a cloud back end, aggregating several trains' state and history
rather than running on any single vehicle.

## What it demonstrates

- Multiple simulated trains on a shared loop route, each with position and
  speed derived consistently from elapsed time (`FleetSimulator::trainStateAt`)
  and a structured, per-train-phased schedule-adherence signal standing in
  for real dwell-time variability — not unstructured noise.
- A live fleet table (`FleetTableModel`) showing position, speed, delay,
  next stop and punctuality status per train.
- A hand-drawn `QPainter` delay/punctuality trend (`DelayTrendWidget`) —
  deliberately not Qt Charts / Qt Graphs, both commercial-only.
- A real SQLite-backed run history (`RunHistoryStore`) via `QtSql`: schema
  `run_history(train_id, timestamp, position_km, speed_kmh, delay_minutes)`,
  parameterized statements, range queries, and a fleet-wide average-delay
  query for punctuality reporting.

## Qt modules exercised

- **Qt SQL** (`Qt6::Sql`, `QSQLITE` driver) — the run history store.
- **Qt Widgets** — the table/trend dashboard layout.
- **Qt Core** — the model classes and the fleet signal generator.

## Architecture

`fleet_ops_lib` holds everything testable without a GUI: `FleetSimulator`
(pure functions of simulated time), `DelayCalculator` (pure punctuality
classification), `RunHistoryStore` (the SQL layer), and `FleetTableModel`.
`MainWindow` and `DelayTrendWidget` — compiled directly into the app target
— own the `QTimer` that advances the fleet simulation once a second,
persists every train's sample, and redraws the trend for the selected train.

## Build & run

```
cmake --build . --target offboard_fleet_ops_center
./industries/railway/offboard-fleet-ops-center/offboard_fleet_ops_center
```

## Tests

```
ctest -R test_fleet_ops_logic
```

Covers punctuality classification thresholds, the fleet simulator's
position/speed/next-stop consistency and even train spacing, a full run
history insert/range-query/average-delay round trip against an in-memory
SQLite database, and the fleet table model.
