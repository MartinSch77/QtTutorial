# Onboard HMI Panel

A fullscreen, kiosk-style Qt Quick plant panel of the kind that would run
embedded on the operator terminal bolted to a skid or control cabinet, not in
a control room. It has no network dependency and persists nothing: everything
on screen is live process state, simulated in-process.

## What it demonstrates

- A mimic-diagram-style HMI (painted tank, pump, pipes — clarity over
  photorealism, same as real SCADA/HMI screens) built with plain `Rectangle`
  and `Item` primitives in QML, animated with `Behavior`/`PropertyAnimation`.
- Believable process dynamics: tank level *integrates* the difference between
  inflow and outflow rather than jumping to a target, and both flow and
  pressure ramp with a first-order lag after the pump starts (`ProcessSimulator::tick`).
- A `QStateMachine`-driven pump lifecycle (Idle → Running → Fault, Fault only
  clearable by an explicit reset) with a *guarded* transition: starting the
  pump is refused while the tank is already critically full
  (`PumpController` / `GuardedSignalTransition`).
- A scrolling, severity-coloured alarm banner and history list backed by a
  real `QAbstractListModel` (`AlarmLogModel`).

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
properties.

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

Covers the process dynamics (integration, ramping, alarm thresholds), the
pump state machine including the guarded transition and fault/reset cycle,
and the alarm log model — all without needing a QML engine.
