# Offboard Central Command Dashboard

The offboard counterpart to the onboard multi-platform HMI: a command-center
dashboard aggregating a mixed simulated fleet (vehicles, drones, a vessel and
static installations) on a top-down tactical map, a status table, a
severity-ordered event/alert log, and SQLite-backed asset status history.

## Safety framing — read this first

**This is passive display/visualization software with simulated data. It
has no weapons, targeting, or fire-control capability, by design.** The
tactical map is a plain 2D plot of each asset's last-known position, type
and health — colored icons and labels, no map imagery, no engagement
geometry, no weapons-range rings, and no target cueing of any kind. The
alert log reports simulated health degradations (e.g. "VEH-02 health
degraded to Caution"), never a targeting or engagement recommendation.
Deliberately omitted: any weapons guidance/targeting algorithm, fire-control
computation, or munitions trajectory/aiming logic.

## What it demonstrates

- **A believable mixed-fleet simulation.** `FleetSimulator` moves mobile
  assets (vehicles, drones, a vessel) with simple heading/speed kinematics
  while static installations stay put, and runs a gentle random-walk health
  metric per asset — one vehicle is seeded with a slow one-directional drift
  fault so the alerting has something genuine to escalate through
  Caution into Critical, the same technique the avionics and space offboard
  examples use.
- **Severity-ordered alerting.** `AlertLog` is a small, pure C++23 class
  that stores alerts and returns them ordered Critical-first, then most
  recent first within the same severity — unit-testable without Qt Widgets.
- **A hand-painted tactical map.** `TacticalMapWidget` draws range rings and
  a distinct icon shape per asset type (circle/triangle/diamond/square),
  colored by health, entirely with `QPainter` — no Qt Charts/Graphs (both
  GPL/commercial-only) and no map-tile imagery.
- **SQLite-backed history via QtSql with parameterized queries.**
  `AssetHistoryStore` writes and reads exclusively through
  `QSqlQuery::prepare`/`bindValue`, following the same pattern as
  `industries/space/offboard-mission-control/src/TelemetryHistoryStore`.
- **A second, independent simulated fault.** `CommsLinkQualityModel` +
  `FleetSimulator` model a per-asset comms-link quality that, when it
  degrades, makes that asset's displayed position/heading freeze and age
  rather than update every tick — a data-quality effect only, distinct
  from the health-drift fault, surfaced on the tactical map (dashed
  outline, warning-triangle glyph, antenna glyph) and in the asset table's
  Data Link column.
- **A fleet-readiness summary board.** `FleetReadiness::summarizeReadiness`
  is a small, pure counting function (unit-tested independent of Qt
  Widgets); `FleetReadinessBoard` renders it as three green/amber/red tiles
  with a procedurally-drawn shield glyph per tile.
- **A severity-filterable alert log.** `AlertLog::alertsBySeverity(minimumSeverity)`
  filters the same Critical-first ordering to a chosen minimum severity,
  backing a filter dropdown above the dashboard's alert list.
- **The offboard characteristic.** Table/map/log oriented, persists to
  SQLite, aggregates a mixed fleet rather than representing a single
  platform's onboard displays — the kind of system that runs in a command
  center, not aboard any one vehicle.

## Qt modules used

- **Qt Widgets** for the dashboard (`QTableView` + `QAbstractTableModel`,
  `QListWidget` for the alert log, and the hand-painted tactical map
  widget).
- **Qt Sql** (`QSqlDatabase`, `QSqlQuery`, SQLite driver) for asset status
  history, with all statements parameterized.
- **Qt Core** (`QTimer`, signals/slots) to drive the periodic
  simulation/persistence tick.

## Process note

Real defence command-and-control software is often governed by
domain-specific standards analogous to safety/airworthiness standards such
as DO-178C for airborne systems. This repository does not claim any such
compliance — it is a technology showcase, not a certified artifact. Where a
life-cycle process is referenced generically elsewhere in this repository,
it is **ISO/IEC/IEEE 12207**.

## Build & run

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target offboard_central_command
./build/industries/defence/offboard-central-command/offboard_central_command
```

## Tests

`AlertLog`, `FleetSimulator` and `AssetHistoryStore` are unit-tested with
QTest under `tests/industries/defence/offboard-central-command/`.
