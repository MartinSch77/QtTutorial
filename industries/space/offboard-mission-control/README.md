# Offboard Mission Control Dashboard

The offboard counterpart to the onboard satellite telemetry panel: a
ground-based mission control dashboard tracking a small fleet of satellites'
orbital position, battery state, and subsystem health summary, with
SQLite-backed telemetry history.

## What it demonstrates

- **A fleet-consistent orbit/power model.** `FleetOrbitSimulator` advances
  each satellite's orbital phase at its own rate and derives its eclipse state
  from that phase, then integrates battery state-of-charge the same way the
  onboard app does — charging only while sunlit, discharging continuously
  against a bus load — so a satellite's battery, phase and eclipse state are
  never inconsistent with each other. Pure C++23, unit-testable without Qt
  Widgets or a running application.
- **A simple, explainable health classifier.** `classifyHealth()` combines
  battery level and eclipse state into Nominal/Caution/Critical, used both in
  the fleet table and (in the same style as the onboard app) could back a
  state machine if extended.
- **SQLite-backed history via QtSql with parameterized queries.**
  `TelemetryHistoryStore` writes and reads exclusively through
  `QSqlQuery::prepare`/`bindValue`.
- **A hand-painted trend view.** `BatteryTrendWidget` draws the selected
  satellite's battery percentage over its recent simulated pass (QPainter, not
  Qt Charts/Graphs — both GPL/commercial-only), shading eclipse periods.
- **The offboard characteristic.** Table/report oriented, persists to SQLite,
  aggregates a fleet rather than representing a single vehicle's cockpit — the
  kind of system that runs in a control room, not onboard any one satellite.

## Qt modules used

- **Qt Widgets** for the dashboard (`QTableView` + `QAbstractTableModel`, plus
  the hand-painted trend widget).
- **Qt Sql** (`QSqlDatabase`, `QSqlQuery`, SQLite driver) for telemetry
  history, with all statements parameterized.
- **Qt Core** (`QTimer`) to drive the periodic simulation/persistence tick.

## Process note

Real mission-control ground segment software is typically developed under
standards such as the **ECSS** software engineering standards; this
repository does not claim any such compliance — it is a technology showcase.
Where a life-cycle process is referenced generically elsewhere in this
repository, it is **ISO 12207**.

## Build & run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target offboard_mission_control
./build/industries/space/offboard-mission-control/offboard_mission_control
```

## Tests

`classifyHealth()`, `FleetOrbitSimulator` and `TelemetryHistoryStore` are
unit-tested with QTest under
`tests/industries/space/offboard-mission-control/`.
