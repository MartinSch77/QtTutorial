# Onboard HMI Panel

A fullscreen, kiosk-style Qt Quick plant panel of the kind that would run
embedded on the operator terminal bolted to a skid or control cabinet, not in
a control room. It has no network dependency and persists nothing: everything
on screen is live process state, simulated in-process.

## Design reference

Visually styled after the genre of embedded plant HMI screens popularised by
products like **Siemens WinCC** and **Rockwell FactoryTalk View** — a dark
theme, colour-coded run/stop/fault equipment, and a P&ID-style process line
(tank → valve → conveyor → motor) with a docked alarm list. This is a
*style/genre reference only*: no Siemens/Rockwell trademark, logo, wordmark,
or exact layout is reproduced. Every glyph on screen (gear, valve, motor,
conveyor, tank/silo) is drawn from scratch as vector shapes in this app's own
QML `Canvas` components, not an imported icon font, image, or SVG asset.

## What it demonstrates

- A mimic-diagram-style HMI, now a full P&ID-style process line — tank →
  valve → conveyor → motor — assembled from procedurally hand-drawn icon
  components (`TankIcon`, `ValveIcon`, `ConveyorIcon`, `MotorIcon`,
  `GearIcon`), each painted with `Canvas` (arcs, lines, trigonometry-computed
  gear teeth), animated with `Behavior`/`PropertyAnimation`/`RotationAnimation`.
- Believable, cross-coupled process dynamics, not just one isolated loop:
  tank level *integrates* the difference between inflow and outflow rather
  than jumping to a target; flow and pressure ramp with a first-order lag
  after the pump starts; the downstream **valve gates the tank's outflow**
  (closing it stops the tank from draining, exactly like a real block valve);
  and the **conveyor motor only runs while the valve is open and the pump is
  feeding flow**, ramping its speed rather than switching it instantly
  (`ProcessSimulator::tick`).
- A `QStateMachine`-driven pump lifecycle (Idle → Running → Fault, Fault only
  clearable by an explicit reset) with a *guarded* transition: starting the
  pump is refused while the tank is already critically full
  (`PumpController` / `GuardedSignalTransition`).
- A severity-coloured alarm **acknowledgement** panel (`AlarmAckPanel.qml`),
  not just a read-only log: each alarm can be acknowledged individually or
  all at once, backed by a real `QAbstractListModel` (`AlarmLogModel`) that
  tracks an outstanding/unacknowledged count, mirroring the "see it, ack it,
  clear it" workflow a real HMI alarm banner drives an operator towards.

## Qt modules exercised

- **Qt Quick / QML** — the kiosk UI itself.
- **Qt State Machine** (`Qt6::StateMachine`) — the pump lifecycle.
- **Qt Core** — `QTimer`-driven simulation loop, property/signal plumbing.

## Architecture

`onboard_hmi_panel_lib` is a static library holding all the testable logic —
`ProcessSimulator`, `PumpController`, `GuardedSignalTransition`, and
`AlarmLogModel` — with no dependency on QML. `HmiApp` (compiled directly into
the app target, like `MainWindow` in the widgets-basics example) wires those
pieces together — pump faults raise alarms, high-level alarms trip the pump
into Fault — and `main.cpp` exposes the three subsystems to QML as context
properties. The valve/conveyor/motor coupling and the alarm acknowledgement
state live in the same testable classes (`ProcessSimulator`, `AlarmLogModel`)
rather than in QML, so they're exercised by `test_hmi_logic` without an
engine; the icon components (`TankIcon.qml`, `ValveIcon.qml`,
`ConveyorIcon.qml`, `MotorIcon.qml`, `GearIcon.qml`) and `AlarmAckPanel.qml`
are display-only, bound to those C++ properties.

## Build & run

From the repository's build directory (configured per the root `README`):

```
cmake --build . --target onboard_hmi_panel
./industries/industrial-automation/onboard-hmi-panel/onboard_hmi_panel
```

## Tests

```
ctest -R test_hmi_logic
```

Covers the process dynamics (integration, ramping, alarm thresholds, the
valve gating downstream outflow, the motor/conveyor only running when the
valve is open and the pump is feeding flow), the pump state machine
including the guarded transition and fault/reset cycle, and the alarm log
model including per-alarm and "acknowledge all" acknowledgement — all
without needing a QML engine.
