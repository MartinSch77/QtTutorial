# Onboard Wall Panel (Home Automation)

A kiosk-style Qt Quick wall panel that controls several device categories
from one screen, the way a single "smart home remote" is supposed to work:
representative of software that would run embedded on the physical panel
mounted at home, at the point of control — no network, no persistence, just
direct, immediate control of the devices in the house.

## What it demonstrates

- A sidebar-selected, tabbed layout (Lighting / Climate / Locks / Blinds /
  Security) so all five device categories fit cleanly in one application,
  matching the "control all devices with one application" goal rather than
  splitting them across several narrow apps.
- Room-by-room lighting (on/off + brightness slider, a handful of rooms) and
  window blind/shade position sliders, both backed by `RoomRegistry`, which
  holds and clamps each room's state.
- A thermostat (`ClimatePanel.qml`/`ThermostatGauge.qml`) with a hand-painted
  `Canvas` arc gauge showing current vs. target temperature, backed by
  `ThermostatModel`: a small simulated thermal model that moves the current
  temperature toward the target at a fixed rate while actively heating or
  cooling, and drifts back toward an "outside" ambient temperature when the
  system is off — it never snaps instantly, and a heater never actively
  cools (or vice versa).
- Door locks (locked/unlocked per entry) backed by `LockRegistry`, which
  keeps a "last changed by" audit log of real state changes only (toggling a
  lock to the state it is already in logs nothing).
- A security/camera status tile grid backed by `SecurityCenter`: an
  armed/disarmed toggle plus a few simulated door/window/motion sensors, with
  a "breach" only ever considered to exist while the system is armed.
- `RoomListModel`, `LockListModel`, `SecurityListModel` and
  `ThermostatController` are thin `QAbstractListModel`/`QObject`+
  `QML_ELEMENT` façades that expose the above to QML and (for the security
  panel and thermostat) step a simulation on a `QTimer`; all of the actual
  state, clamping and thermal-model logic lives in
  `ThermostatModel`/`RoomRegistry`/`LockRegistry`/`SecurityCenter`, plain C++
  classes with no Qt GUI dependency, so they are unit tested headlessly.

## Qt modules/APIs exercised

- **Qt Quick / QML** for a GPU-accelerated, low-footprint HMI suitable for a
  wall-mounted touch panel.
- **Qt Quick `Canvas`** for the thermostat gauge, avoiding any charting
  library (Qt Charts/Graphs are not MIT licensed).
- **`QML_ELEMENT`** / `qt_add_qml_module` for direct C++-to-QML type
  registration of the list models and thermostat controller.
- **`QAbstractListModel`** for the room/lock/sensor lists driving `Repeater`s
  in each panel.
- **`QTimer`** driving the thermostat's simulated thermal steps and the
  security panel's simulated sensor activity, standing in for a real
  building-automation bus.

This example intentionally has **no** `Qt6::Network` or `Qt6::Sql` dependency
(see REQ-IND-04 in `requirements/requirements.md`): the wall panel is the
source of truth for the home's state, controlled directly with no network
round trip. Its offboard counterpart,
`industries/home-automation/offboard-remote-access-app/`, is the one that
talks to it over the network and persists history.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target onboard_wall_panel
./build/industries/home-automation/onboard-wall-panel/onboard_wall_panel
```

## Tests

See `tests/industries/home-automation/onboard-wall-panel/`, which covers
`ThermostatModel`'s heat/cool/off behaviour and rate, `RoomRegistry`'s
lighting/blind toggling and clamping, `LockRegistry`'s change log, and
`SecurityCenter`'s armed/breach logic — all without any Qt GUI dependency.
