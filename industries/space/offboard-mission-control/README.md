# Offboard Mission Control Dashboard

The offboard counterpart to the onboard satellite telemetry panel: a
ground-based mission control dashboard tracking a small fleet of satellites'
orbital position, battery state, and subsystem health summary, with
SQLite-backed telemetry history, a world ground-track map, and a next
-contact-window schedule.

## Design reference

The dark background, colour-coded fleet health, world ground-track map and
"next contact" schedule take their **visual language** from the genre of
NASA JPL/ESA mission-control room displays - dark panels, a world map with
live orbital tracks, red/amber/green status. This is a style/genre reference
only: no NASA, ESA, JPL, or any other agency's trademark, logo, wordmark,
exact layout, or copyrighted asset is reproduced anywhere in this app. The
world map's landmass shapes are simple, deliberately non-precise stylised
polygons drawn with `QPainterPath` for visual orientation, not sourced from
any mapping/GIS dataset. All icons are procedurally drawn `QPainterPath`
vector shapes (see `IconPainter`), not fetched or adapted from any external
image, icon font, or SVG asset. The ground stations plotted on the map are
fictitious, not real facilities.

## What it demonstrates

- **A fleet-consistent orbit/power/ground-track model.** `FleetOrbitSimulator`
  advances each satellite's orbital phase at its own rate and derives its
  eclipse state from that phase, then integrates battery state-of-charge the
  same way the onboard app does — charging only while sunlit, discharging
  continuously against a bus load — so a satellite's battery, phase and
  eclipse state are never inconsistent with each other. It also derives each
  satellite's sub-satellite latitude/longitude from its phase and its own
  fixed inclination via the shared `GroundTrackMath` functions (each fleet
  member flies a different inclination, so their ground tracks visibly
  differ). Pure C++23, unit-testable without Qt Widgets or a running
  application.
- **A simple, explainable health classifier.** `classifyHealth()` combines
  battery level and eclipse state into Nominal/Caution/Critical, used both in
  the fleet table, the world map satellite colouring, and (in the same style
  as the onboard app) could back a state machine if extended.
- **A next-contact-window predictor.** `GroundStationTracker` holds a small
  fixed set of fictitious ground stations and, for a satellite not currently
  in view of any of them, forward-samples the same ground-track model to
  estimate when and at which station it will next come into view — pure
  C++23, independently unit-tested, no Qt Widgets dependency.
- **SQLite-backed history via QtSql with parameterized queries.**
  `TelemetryHistoryStore` writes and reads exclusively through
  `QSqlQuery::prepare`/`bindValue`.
- **Hand-painted trend, map and schedule views.** `BatteryTrendWidget` draws
  the selected satellite's battery percentage over its recent simulated pass;
  `WorldMapWidget` draws a flattened equirectangular world map (graticule,
  stylised landmasses, ground stations, and each satellite's current position
  plus a fading recent ground track); `PassScheduleWidget` lists each
  satellite's next contact window. All QPainter, not Qt Charts/Graphs (both
  GPL/commercial-only), and all icons are procedurally-drawn `QPainterPath`
  vector shapes (see `IconPainter`), not external assets.
- **The offboard characteristic.** Table/map/report oriented, persists to
  SQLite, aggregates a fleet rather than representing a single vehicle's
  cockpit — the kind of system that runs in a control room, not onboard any
  one satellite.

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

`classifyHealth()`, `FleetOrbitSimulator`, `TelemetryHistoryStore`,
`GroundTrackMath` (`computeGroundTrack()`/`angularDistanceDeg()`) and
`GroundStationTracker` are unit-tested with QTest under
`tests/industries/space/offboard-mission-control/`.
