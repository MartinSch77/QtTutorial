# Offboard Remote Access App (Home Automation)

A phone-shaped Qt Quick app for controlling the same home devices as
`onboard-wall-panel` while away from home: the same device categories, in a
more compact, list-based layout, talking to the home system over
`QtNetwork` rather than controlling it directly — the "app on your phone"
counterpart to the "panel physically on your wall".

## Design reference

The card/list styling, the per-device icons (lightbulb, lock, thermostat
dial, door/window sensor), and the activity-timeline-plus-trend-chart layout
take their visual language from the room-by-room dashboard genre popularized
by apps like Google Home, Apple Home, and Control4 - dark cards, a colored
accent per device state, a history feed with a small icon per entry. This is
a **style/genre reference only**: nothing here reproduces any of those
products' logos, wordmarks, exact layouts, or other trademarked assets.
Every icon is drawn from scratch as plain geometric paths on a Qt Quick
`Canvas` (`LightbulbIcon.qml`, `LockIcon.qml`, `DoorSensorIcon.qml`,
`ThermostatIcon.qml`, `ActivityIcon.qml`) - no icon font, image, or SVG asset
is fetched from anywhere.

## What it demonstrates

- `HomeLinkClient`/`HomeLinkServer` speak a small newline-delimited JSON
  protocol (`Protocol.h`), the same framing style as
  `framework-tour/04-networking/src/Protocol.h`: a `Command` sent from the
  remote app to the home (`setLight`, `setBrightness`, `setBlind`,
  `setLock`, `setArmed`, `setThermostatTarget`, `setThermostatMode`), and a
  `Snapshot` of the whole home's state pushed back whenever anything
  changes.
- `HomeSimulator` stands in for "the house" on the server side: it applies
  incoming commands (clamping values, ignoring no-op or unknown commands)
  and holds the current lighting/blind/lock/security/thermostat state.
- `SnapshotDiff::describeChanges` compares the previous and current snapshot
  and produces one human-readable description per real change (e.g.
  `"Living Room lights turned on"`), which is what actually drives the
  activity log — not a raw dump of every field on every push.
- `ActivityLogStore` persists those descriptions to a SQLite database via
  `QtSql` using parameterized queries, so the app can show "what happened
  while I was away". The shipped app uses an in-memory database for a
  self-contained demo.
- `RemoteHomeController` is the `QObject`/`QML_ELEMENT` façade that owns the
  network client (and, for this self-contained demo, also hosts a local
  `HomeLinkServer` standing in for the home hub the client would otherwise
  reach over the internet), republishes the latest snapshot as QML-friendly
  properties, and turns state changes into activity-log entries.
- `EnergyMonitor::estimateWatts` derives an estimated instantaneous power
  draw from each incoming `Snapshot` (a household base load, plus a
  per-light contribution that scales with brightness while a room's light
  is on, plus a flat contribution while the thermostat is actively heating
  or cooling) - a simple but directionally correct model that makes the
  same state changes a user can already see (lights off, heating set back)
  visibly cheaper. `EnergyHistory` keeps a fixed-capacity rolling window of
  those estimates in memory (not SQLite - it is a derived/estimated value,
  not durable history) for the new **Energy** trend chart
  (`EnergyPanel.qml`), a hand-painted Canvas line chart in the same style as
  the wall panel's `ThermostatGauge.qml` - this app still links no Qt
  Charts/Graphs module.
- The **Activity** timeline now shows a small procedurally-drawn icon per
  entry (`ActivityIcon.qml`, dispatching by keyword on the description
  `SnapshotDiff::describeChanges` already produces) instead of plain text
  lines, and the room/lock/sensor rows each carry a matching device icon.

## Qt modules/APIs exercised

- **Qt Quick / QML** for a lightweight app UI, matching the wall panel's
  visual style at a smaller, list-based scale.
- **Qt Quick `Canvas`** for the energy trend chart and every device/activity
  icon, hand-painted vector paths rather than an icon font, bitmap asset, or
  charting library (Qt Charts/Graphs are not MIT licensed and are not
  linked here).
- **`QtNetwork`** (`QTcpSocket`, `QTcpServer`) for the client/server link to
  the home system.
- **`QtSql`** (`QSqlDatabase`, `QSqlQuery`) for the activity log, using
  parameterized `INSERT`/`SELECT` statements throughout.
- **`QML_ELEMENT`** / `qt_add_qml_module` for direct C++-to-QML type
  registration of `RemoteHomeController`.

This example is permitted to (and does) link against `Qt6::Network` and
`Qt6::Sql` (see REQ-IND-05 in `requirements/requirements.md`): unlike
`onboard-wall-panel`, which is the source of truth for the home's state and
has no network or persistence dependency at all (REQ-IND-04), this app's
whole job is talking to that state over a network and remembering its
history.

## The wire protocol

Every message is one JSON object per line ("newline-delimited JSON"):

- **Command** (`{"type": "...", "target": "...", "value": 0}`) — sent from
  the remote app to the home. `target` names the affected room/lock and is
  empty for `setArmed`; `value` carries a brightness/position/temperature or
  a boolean encoded as `0`/`1`.
- **Snapshot** (`{"rooms": [...], "locks": [...], "armed": false, "sensors":
  [...], "thermostatMode": 1, "thermostatCurrent": 19.5, "thermostatTarget":
  21.0}`) — pushed from the home to every connected client whenever anything
  changes, and periodically to reflect simulated sensor activity.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target offboard_remote_access_app
./build/industries/home-automation/offboard-remote-access-app/offboard_remote_access_app
```

## Tests

See `tests/industries/home-automation/offboard-remote-access-app/`, which
covers `HomeSimulator`'s command-toggling logic (including clamping and
no-ops), `SnapshotDiff::describeChanges`'s change detection,
`ActivityLogStore`'s insert/query behaviour against an in-memory SQLite
database, and `EnergyMonitor`/`EnergyHistory`'s power-estimate model
(base load, per-light and thermostat contributions, and that an
all-off/thermostat-off snapshot draws less than an everything-on one) and
rolling-window behaviour — no UI or live network dependency.
