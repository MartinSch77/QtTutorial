# Space

Two example applications mirror the avionics pair, this time for a
spacecraft: an **onboard** subsystem telemetry panel and an **offboard**
mission-control dashboard.

## Onboard vs. offboard, concretely

- **Onboard** (`industries/space/onboard-satellite-telemetry/`) is software
  that would run *inside* a satellite or spacecraft: a fullscreen,
  kiosk-style panel showing orbital parameters, battery state-of-charge,
  thermal zone temperatures, and a subsystem health grid. It has no network
  connection and no persistence — everything comes from an in-process
  orbital/power/thermal simulation driven by a `QTimer`, with subsystem health
  states tracked by real `QStateMachine` instances. That is representative of
  onboard spacecraft software: closed, real-time, running on whatever compute
  the vehicle carries with it.
- **Offboard** (`industries/space/offboard-mission-control/`) is software that
  runs *on the ground*: a mission-control dashboard tracking a small fleet of
  satellites' orbital position, battery percentage and health summary in a
  table, a hand-painted trend view of a selected satellite's battery over its
  most recent pass, and SQLite-backed telemetry history. It is table/report
  oriented and exists to support ground-segment decisions across a fleet, not
  to run any one spacecraft's subsystems.

The contrast is the point: the onboard app is the kind of software that would
fly on the vehicle and be constrained by its compute and power budget; the
offboard app is ordinary IT infrastructure a mission-control operator uses at
a console.

## What each app demonstrates

**Onboard Satellite Telemetry & Subsystem Panel**
- An internally consistent orbit/power/thermal model, all pure C++23 with no
  Qt dependency: `OrbitalSimulator` sweeps true anomaly and derives eclipse
  phases from it; `PowerSystem` charges the battery only when there is solar
  input *and* the panels are sun-pointed, and always discharges against a
  constant bus load; `ThermalSimulator` drifts four zones toward
  solar-input-dependent targets, each with its own thermal time constant.
- A genuine Qt State Machine: `SubsystemHealthMachine` wires three `QState`
  objects (Nominal/Caution/Critical) together with `QState::addTransition`,
  driven by a `SubsystemMonitor` that classifies a parameter against
  nominal/critical bands and emits edge-triggered signals. Four of these back
  the subsystem health grid.
- Hand-painted radial gauges for battery SoC and each thermal zone.

**Offboard Mission Control Dashboard**
- A fleet-consistent orbit/power simulation (`FleetOrbitSimulator`) using the
  same charge-only-when-sunlit, discharge-always logic as the onboard app, so
  a satellite's battery, phase and eclipse state are never inconsistent with
  each other.
- SQLite-backed telemetry history via QtSql, written and read exclusively
  through parameterized `QSqlQuery::prepare`/`bindValue` statements.
- A hand-painted battery trend chart for the selected satellite, shading
  eclipse periods.

## Qt modules used and why

- **Qt Widgets** — both apps are Widgets-based, for the same reasons as the
  avionics pair: a fullscreen custom-painted panel for the onboard app, a
  table/report dashboard for the offboard app. Neither uses Qt Charts or Qt
  Graphs (both GPL/commercial-only); gauges and trend views are hand-painted
  with `QPainter`, keeping the repository MIT-licensed.
- **Qt State Machine** (`Qt6::StateMachine`) — used by the onboard app for
  subsystem health tracking. Spacecraft subsystem management is naturally
  state-machine shaped (nominal/degraded/safe modes), and Qt ships a real
  hierarchical state machine framework for exactly this rather than requiring
  a hand-rolled enum switch.
- **Qt Core** (`QTimer`, signals/slots) — drives both apps' simulation loops.
- **Qt Sql** (offboard only) — SQLite-backed telemetry history, the
  durability layer the no-persistence onboard app has no reason to carry.

## Process note

Real spacecraft software is typically developed under standards such as the
**ECSS** software engineering standards. This repository does not claim any
such compliance — it is a technology showcase. Where this repository
references a software life-cycle process generically, it uses **ISO 12207**.
