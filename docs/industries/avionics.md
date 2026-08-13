# Avionics

Two example applications show the two very different places Qt code runs in
an aircraft program: **onboard** the aircraft itself, and **offboard** in the
airline's operations back office.

## Onboard vs. offboard, concretely

- **Onboard** (`industries/avionics/onboard-primary-flight-display/`) is
  software that would run *inside* the aircraft, on flight-deck display
  hardware: a fullscreen, kiosk-style Primary Flight Display with an
  attitude indicator, airspeed/altitude tapes, a heading indicator and a
  vertical-speed indicator. It has no network connection and no persistence —
  everything it shows comes from a self-contained, in-process flight-dynamics
  simulation driven by a `QTimer`. That is a deliberate and accurate
  reflection of embedded flight-deck software: closed, real-time, and not
  dependent on anything outside the box.
- **Offboard** (`industries/avionics/offboard-fleet-maintenance/`) is
  software that runs *away* from any single aircraft: an airline maintenance
  operations dashboard that aggregates simulated telemetry from a small fleet,
  flags predictive-maintenance conditions, tracks a maintenance task list, and
  persists parameter history to a SQLite database. It is table/report
  oriented and exists to support decisions across many aircraft, not to fly
  one.

The contrast is the point: the onboard app could plausibly be certified
avionics software running on a single airframe; the offboard app is an IT
system a maintenance controller uses at a desk.

## What each app demonstrates

**Onboard Primary Flight Display**
- A custom-painted artificial horizon using real `QPainter` transforms
  (rotate for roll, translate for pitch) rather than a static image — the
  canonical hard Qt-painting exercise.
- A small, coupled flight-dynamics model (`FlightDynamics`, pure C++23, no Qt
  dependency): control inputs drive damped pitch/roll responses that in turn
  drive heading rate, vertical speed and airspeed, so every instrument moves
  consistently with the others.
- Scrolling vertical tapes (airspeed, altitude), a rotating compass card, and
  a vertical-speed ladder, all hand-painted.

**Offboard Fleet Maintenance Dashboard**
- A real (if simplified) predictive-maintenance heuristic (`EngineTrend`):
  flags a parameter when it is out of its nominal band or when a
  least-squares trend slope projects it out of band soon.
- A believable fleet simulation with a couple of aircraft seeded with a slow
  directional drift fault, so the detector has something genuine to catch.
- SQLite-backed flight/parameter history via QtSql, written and read
  exclusively through parameterized `QSqlQuery::prepare`/`bindValue`
  statements.

## Qt modules used and why

- **Qt Widgets** — both apps are Widgets-based; the onboard app because a
  fullscreen custom-painted instrument panel is exactly the kind of UI Widgets
  plus `QPainter` was built for, and the offboard app because a table/report
  dashboard is Widgets' home turf (`QAbstractTableModel` + `QTableView`).
  Neither app uses Qt Charts or Qt Graphs — both are GPL/commercial-only —
  instead using hand-painted `QPainter` instruments and trend widgets, which
  keeps the whole repository MIT-licensed.
- **Qt Core** (`QTimer`, signals/slots) — drives both apps' simulation loops
  without blocking the UI thread.
- **Qt Sql** (offboard only) — SQLite-backed persistence for maintenance
  history, the durability layer an onboard, no-persistence app has no reason
  to carry.

## Process note

Real avionics software is typically developed and certified under
**DO-178C**. This repository does not claim any such compliance — it is a
technology showcase, not a certified artifact. Where this repository
references a software life-cycle process generically, it uses **ISO 12207**.
