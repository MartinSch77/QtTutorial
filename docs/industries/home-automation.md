# Home Automation

Two example applications show the same "onboard/offboard" split seen across
this repository's industry showcases, mapped onto the most literal version
of that split available: the panel physically mounted on your wall, and the
app on your phone that talks to it over the network.

## Onboard vs. offboard, concretely

- **Onboard — `industries/home-automation/onboard-wall-panel/`.** This is
  what would run embedded on the wall panel itself: a single application
  controlling every device category in the house — room-by-room lighting
  and blinds, a thermostat, door locks, and a security/sensor grid — from a
  sidebar-selected panel. It is the source of truth for the home's state,
  controlled directly with no network round trip and nothing to persist
  between sessions; that is what makes the "control all devices with one
  application" goal literal here, not just a slogan.
- **Offboard — `industries/home-automation/offboard-remote-access-app/`.**
  This is what would run on your phone while you are away: the same device
  categories, in a more compact, list-based layout, talking to the home
  system over `QtNetwork` with a small JSON protocol and keeping a
  `QtSql`-backed activity log of what changed while you were not there. It
  exists because a wall panel has no reason to expose itself to the
  internet or to remember history — that is exactly the kind of
  responsibility this repository's onboard/offboard convention pushes to the
  offboard side.

## Why the underlying simulation matters

The onboard app's `ThermostatModel` is a small simulated thermal model, not
a random-walk or an instant snap to the target: it moves the current
temperature toward the target at a fixed rate while actively heating or
cooling (and only while the room is on the wrong side of the target — a
heater never actively cools), and drifts back toward an "outside" ambient
temperature when the system is off, at a deliberately slower rate than the
active heat/cool rate. `RoomRegistry`, `LockRegistry` and `SecurityCenter`
provide the toggling logic and clamping for lighting/blinds, locks (with a
"last changed by" log that only records real state changes, not repeated
no-ops), and the armed/breach decision for the security grid (a breach only
exists while armed).

The offboard app's `HomeSimulator` mirrors the same toggling and clamping
logic on the "home system" side of the network link, and
`SnapshotDiff::describeChanges` turns a before/after `Snapshot` comparison
into the same kind of human-readable change descriptions a real remote
app's activity log would show — deliberately excluding sensor blips and the
thermostat's slowly-drifting current temperature, since those are not
"changes a user made" and would flood the log every tick if included.

## Qt modules used, and why they matter here

| App | Modules | Why |
| --- | --- | --- |
| Onboard wall panel | Qt Quick, Qt Quick `Canvas`, Qt Qml | A GPU-accelerated scene graph suits a wall-mounted touch panel; `Canvas` hand-paints the thermostat's arc gauge without a charting dependency. |
| Offboard remote access app | Qt Quick, Qt Qml, QtNetwork, QtSql | A compact, list-based Qt Quick UI fits a phone-shaped window; `QtNetwork` (`QTcpSocket`/`QTcpServer`) carries the newline-delimited JSON protocol to the home system; `QtSql` (SQLite) provides the durable activity log a wall panel has no reason to keep. |

Both logic layers (`ThermostatModel`, `RoomRegistry`, `LockRegistry`,
`SecurityCenter` on the onboard side; `Protocol`, `HomeSimulator`,
`SnapshotDiff`, `ActivityLogStore` on the offboard side) are plain C++ with
no Qt GUI dependency, unit tested with QTest under
`tests/industries/home-automation/`. Where a process standard is relevant to
this kind of software, this repository references **ISO/IEC/IEEE 12207**
(software life cycle processes) rather than any safety-classification
standard — keeping the device-state and thermal-model logic separate from
both the UI and the network transport is what makes that logic
independently reviewable and testable, which is the underlying point of any
such process standard.

## The wire protocol, briefly

`industries/home-automation/offboard-remote-access-app/README.md` documents
the newline-delimited JSON `Command`/`Snapshot` protocol in full; in short, a
`Command` (`type`, `target`, `value`) flows from the remote app to the home,
and a `Snapshot` of the entire home's state flows back whenever anything
changes. The onboard wall panel does not speak this protocol at all — per
REQ-IND-04 in `requirements/requirements.md`, it has no `Qt6::Network` or
`Qt6::Sql` dependency, and the network/persistence side of this showcase is
entirely the offboard app's responsibility (REQ-IND-05).
