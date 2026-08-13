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
- A schematic fleet map (`FleetMapWidget`): the shared loop route drawn as a
  track diagram with station ticks and one procedurally-drawn train-icon
  marker per live train, coloured by that train's punctuality and oriented
  along its direction of travel — positioned via the pure, unit-tested
  `positionOnLoop` geometry function (`FleetMapGeometry`) so the map can
  never disagree with the table about where a train is.
- A hand-drawn `QPainter` delay/punctuality trend (`DelayTrendWidget`) —
  deliberately not Qt Charts / Qt Graphs, both commercial-only — used twice:
  once for whichever train is selected, and once (reusing the same widget)
  for a fleet-wide, bucketed punctuality trend across the whole network.
- A real SQLite-backed run history (`RunHistoryStore`) via `QtSql`: schema
  `run_history(train_id, timestamp, position_km, speed_kmh, delay_minutes)`,
  parameterized statements, range queries, a fleet-wide average-delay query,
  and a bucketed `networkDelaySeries` query (`GROUP BY` on a truncated
  timestamp) behind the network-wide punctuality trend.

## Design reference

The fleet map's train markers reuse the same train-front-silhouette
vocabulary as the onboard cab display's ETCS-DMI-inspired icon set (drawn
here with `QPainterPath` rather than QML `Canvas`, since this app is Qt
Widgets), and the schematic loop-line diagram is in the general style of a
rail operations-centre wallboard/train describer display. This is a
style/genre reference only — no trademark, logo, or specific vendor's exact
wallboard layout is reproduced, and every icon is drawn procedurally rather
than copied from a real product's asset set.

## Qt modules exercised

- **Qt SQL** (`Qt6::Sql`, `QSQLITE` driver) — the run history store.
- **Qt Widgets** — the table/trend dashboard layout.
- **Qt Core** — the model classes and the fleet signal generator.

## Architecture

`fleet_ops_lib` holds everything testable without a GUI: `FleetSimulator`
(pure functions of simulated time), `DelayCalculator` (pure punctuality
classification), `FleetMapGeometry` (pure loop-position-to-point geometry),
`RunHistoryStore` (the SQL layer), and `FleetTableModel`. `MainWindow`,
`DelayTrendWidget` and `FleetMapWidget` — compiled directly into the app
target — own the `QTimer` that advances the fleet simulation once a second,
persists every train's sample, redraws the map and the per-train trend, and
refreshes the network-wide punctuality trend from the bucketed SQL query.

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
position/speed/next-stop consistency and even train spacing, the
loop-position-to-point map geometry, a full run history
insert/range-query/average-delay round trip and the bucketed network delay
series against an in-memory SQLite database, and the fleet table model.
