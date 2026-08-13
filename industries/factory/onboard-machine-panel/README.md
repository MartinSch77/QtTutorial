# Onboard Machine Panel

A fullscreen, kiosk-style Qt Quick touch panel of the kind bolted directly to
a single physical machine on a factory floor — a CNC mill, a packaging line,
an industrial press — rather than a control-room dashboard. It is the "on
the machine itself" companion to the much larger
[`offboard-digital-twin-control-center`](../offboard-digital-twin-control-center)
app elsewhere in this tutorial, but deliberately narrower in scope: one
machine, one operator, one screen, no network dependency, and nothing
persisted. Every session starts fresh with a freshly-simulated feed.

## What it demonstrates

- A `MachineSimulator` that models believable sensor dynamics rather than
  jumping to values: speed ramps toward an operator-set target with a
  first-order lag, and temperature/vibration follow speed with their own lag
  plus small bounded noise, exactly like a real drive/sensor chain would
  (`MachineSimulator::tick`).
- A validated numeric setpoint entry: `MachineSimulator::setTargetSpeed`
  rejects out-of-range values (emitting `setpointRejected`) instead of
  silently clamping or crashing.
- A `QStateMachine`-driven machine lifecycle — Idle → Running → Warning →
  Fault → Idle, with a Paused state reachable from and returning to Running —
  where Warning can clear back to Running on its own but Fault is a hard
  stop only clearable by an explicit operator acknowledgement
  (`MachineStateController`).
- A production-cycle counter and progress bar, plus a rotating activity icon,
  animated with `RotationAnimation` and `Behavior on width`/`NumberAnimation`
  in QML — a 2D representation appropriate for a real embedded touch panel,
  not a 3D scene.
- A small in-memory-only event/fault log backed by a real
  `QAbstractListModel` (`EventLogModel`), matching REQ-IND-04: no
  persistence, nothing survives a restart.

## Qt modules exercised

- **Qt Quick / QML** (+ `QtQuick.Controls` for the setpoint field and
  buttons) — the kiosk UI itself.
- **Qt State Machine** (`Qt6::StateMachine`) — the machine lifecycle.
- **Qt Core** — `QTimer`-driven simulation loop, property/signal plumbing,
  `QRandomGenerator` for sensor noise.

This app intentionally does **not** link `Qt6::Network` or `Qt6::Sql`
(REQ-IND-04): it models a disconnected, embedded single-machine HMI, not a
networked or persistent system.

## Architecture

`factory_machine_panel_lib` is a static library holding all the testable
logic — `MachineSimulator`, `MachineStateController`, and `EventLogModel` —
with no dependency on QML. `MachinePanelApp` (compiled directly into the app
target, like `HmiApp` in `industrial-automation/onboard-hmi-panel`) wires
those pieces together — sensor excursions raise/clear the Warning state and
escalate to Fault, every state transition and sensor excursion is recorded
to the event log — and `main.cpp` exposes the three subsystems to QML as
context properties.

## Build & run

From the repository's build directory (configured per the root `README`):

```
cmake --build . --target onboard_machine_panel
./industries/factory/onboard-machine-panel/onboard_machine_panel
```

## Tests

```
ctest -R test_machine_panel_logic
```

Covers the sensor dynamics (ramping, bounded drift, fault-threshold
triggering, cycle-count advancement), the setpoint validator, the full
machine state machine including the guarded Fault/acknowledge cycle, and the
event log model — all without needing a QML engine.
