# Defence

Two example applications show the two very different places Qt code runs in
a defence situational-awareness program: **onboard** several kinds of
platform, and **offboard** in a command center aggregating them. This is
**passive visualization/status software with simulated data** — a
situational-awareness HMI showcase, not real command-and-control, targeting
or weapons software. See the safety framing below before reading further.

## Safety framing

**Both apps are passive display/visualization software over simulated data.
Neither has, nor is intended to gain, any weapons, targeting, or
fire-control capability.** Specifically omitted, by design, across both
apps:

- Any weapons guidance/targeting algorithm.
- Any fire-control computation.
- Any munitions trajectory/aiming logic.
- Anything that could function as targeting assistance.

Where a real-world requirement in this space could plausibly be read as
"tracking" in the weapons sense (e.g. a radar-style track display), it is
implemented strictly as a **track display**: a passive plot of simulated
sensor tracks (position, heading, a generic classification label) with no
aim/intercept solution and no launch/fire-control logic anywhere in the
code. This is the same category of software Qt has genuine real-world
customers building for defence primes today: command-and-control displays,
sensor visualization, and situational-awareness dashboards, all of which
present information to a human operator without automating the decision to
engage.

## Onboard vs. offboard, applied to several platform types

Real defence situational-awareness software spans many platform types:
tanks, ships, submarines, drones, dismounted soldiers with AR displays, and
static installations. Rather than one small app per platform (which would
be inconsistent with every other industry in this repository, each of which
has exactly two apps), this example covers several platform types through a
single onboard app with a mode/tab switcher, paired with one offboard app
that aggregates all of them:

- **Onboard** (`industries/defence/onboard-multi-platform-hmi/`) is a
  fullscreen, kiosk-style app with four switchable panels, each representing
  a different platform's onboard display:
  - **Vehicle crew display** — stands in for a tank/ship/submarine crew
    station: heading, speed, fuel/power, and a subsystem health grid driven
    by a small `QStateMachine` per subsystem.
  - **Sensor track display** — stands in for a drone/ship/air-defence
    sensor operator's scope: a radar-plot-style `Canvas` view of several
    smoothly-moving simulated tracks.
  - **Drone camera object-detection visualization** — a simulated camera
    feed with animated bounding-box overlays and per-box class/confidence
    labels, representing what a detection pipeline's output looks like
    overlaid on video.
  - **AR HUD overlay display** — stands in for AR glasses / a soldier
    heads-up display: a translucent compass strip, waypoint markers and
    teammate position indicators.

  It has no network connection and no persistence — everything shown comes
  from a single `QTimer`-driven facade over four separate, pure C++23
  simulation classes, consistent with embedded onboard software elsewhere
  in this repository.

- **Offboard** (`industries/defence/offboard-central-command/`) is a
  command-center dashboard aggregating a mixed simulated fleet (vehicles,
  drones, a vessel, and static installations) on a top-down 2D tactical map,
  a status table, a severity-ordered event/alert log, and SQLite-backed
  asset status history. It is table/map/log oriented and exists to support
  situational awareness across many assets, not to represent any one
  platform's onboard cockpit.

## What each app demonstrates

**Onboard Multi-Platform HMI**
- A pure, testable simulation core (`TrackMotionModel`, `VehicleStatusSimulator`,
  `DetectionSimulator`, `HudSimulator`) with no Qt Quick dependency.
- The state-machine-driven health-grid pattern (`SubsystemHealthMonitor` +
  `SubsystemHealthMachine`, using the real `Qt6::StateMachine` module),
  applied to a vehicle crew station's Propulsion/Power/Comms/Sensors grid.
- Smooth, non-teleporting motion: tracks turn at a fixed rate and reflect
  back from the edge of the surveillance area; detection boxes bounce
  within the simulated video frame.
- Hand-painted QML `Canvas` instrumentation (the radar plot and HUD compass
  strip) rather than static images or Qt Charts/Graphs.

**Offboard Central Command Dashboard**
- A believable mixed-fleet simulation (`FleetSimulator`) with one asset
  seeded with a slow drift fault, so the alerting has something genuine to
  escalate.
- Severity-ordered alerting (`AlertLog`), unit-tested independent of Qt
  Widgets.
- A hand-painted `QPainter` tactical map with a distinct icon per asset
  type, colored by health.
- SQLite-backed asset history via QtSql, written and read exclusively
  through parameterized `QSqlQuery::prepare`/`bindValue` statements,
  following the same pattern as `industries/space/offboard-mission-control`.

## Qt modules used and why

- **Qt Quick / Qt Qml / Qt Quick Controls 2** (onboard) — the AR-HUD-style
  overlay and camera-overlay-style panels are a natural fit for Qt Quick's
  compositing model; a tab bar switches between the four platform panels.
- **Qt Widgets** (offboard) — a table/map/log dashboard is Widgets' home
  turf (`QAbstractTableModel` + `QTableView`, `QListWidget`, and a
  `QPainter`-based map widget).
- **Qt StateMachine** (onboard) — drives the subsystem health grid.
- **Qt Sql** (offboard only) — SQLite-backed asset status history.
- **Qt Core** (`QTimer`, signals/slots/properties) — drives both apps'
  simulation loops without blocking the UI thread.
- Neither app uses Qt Charts or Qt Graphs — both are GPL/commercial-only —
  instead using hand-painted `QPainter`/`Canvas` instruments, which keeps the
  whole repository MIT-licensed.

## Process note

Real defence software is often governed by domain-specific standards
analogous to safety/airworthiness standards such as **DO-178C** for airborne
systems. This repository does not claim any such compliance for either app
— they are technology showcases, not certified artifacts. Where this
repository references a software life-cycle process generically, it uses
**ISO/IEC/IEEE 12207**.
