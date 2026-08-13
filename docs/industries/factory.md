# Factory

Two example applications for a smart-factory digital-twin scenario: an
**onboard** single-machine touch panel, and an **offboard** control-center
app that visualizes the whole factory floor in 3D.

- **Onboard** (`industries/factory/onboard-machine-panel/`) is a fullscreen
  touch panel bolted to one physical machine — MIT-licensed, no networking,
  no persistence, consistent with every other onboard app in this
  repository. It shows the machine's current state (Idle/Running/Warning/
  Fault, driven by a real `Qt6::StateMachine`), live simulated sensor gauges
  (temperature, RPM, vibration, cycle count), an operator control panel
  (Start/Pause/Stop, a validated setpoint field, an acknowledge-alarm
  button), an animated cycle-progress indicator, and an in-memory event/fault
  log.

- **Offboard** (`industries/factory/offboard-digital-twin-control-center/`,
  "Qt Nexus — Digital Twin Control Center") is a supervisory control room:
  a large interactive 3D factory scene (procedural Qt Quick 3D geometry, PBR
  materials, `View3D::pick` machine selection with a smooth camera fly-to),
  live charts, an alarm/event timeline, a `QStateMachine`-driven maintenance
  workflow, and an automated "Play demo" button that walks through the whole
  overheat → inspect → diagnose → repair scenario in about 90 seconds. **This
  directory is `GPL-3.0-or-later`, not this repository's default MIT** —
  see its own `NOTICE.md` for exactly which Qt modules (Qt Quick Timeline,
  Qt Graphs, Qt Quick 3D Physics) force that, and `docs/qa/licensing.md` for
  the repository-wide picture.

## What each app demonstrates

**Onboard Machine Panel**
- A pure, testable `MachineSimulator` (speed ramping toward an operator
  setpoint, temperature/vibration lag with bounded noise, fault-threshold
  detection) with no Qt Quick dependency.
- A `QStateMachine`-driven lifecycle (Idle → Running → Warning → Fault →
  Idle, plus Pause), with Fault clearing only via an explicit acknowledge.
- An in-memory-only event/fault log (`EventLogModel`), per this repository's
  onboard rule against persistence.

**Offboard Digital Twin Control Center**
- A procedural Qt Quick 3D factory scene built entirely from built-in
  primitive meshes (no external asset files), with real PBR materials and
  `View3D::pick`-based selection.
- Qt Quick 3D Particles for a heat-shimmer overheat effect, and a
  hand-rolled `QStateMachine` maintenance workflow with working cancel/
  retry/error-injection.
- An in-process `MachineTelemetrySimulator` standing in for a real
  MQTT/OPC UA/gRPC industrial link.
- A documented set of "attempt the newest Qt module first, fall back to a
  verified alternative if it isn't available in this build" decisions for
  every Technology-Preview/newest module named in the original spec (Qt
  Graphs, Qt Quick 3D Physics, Qt Multimedia/SpatialAudio, Qt Mqtt, Qt
  TaskTree, Qt Labs StyleKit, Qt Canvas Painter) — see the app's own
  `README.md` for the full table of what was actually verified to exist,
  what built in this sandbox, and what the fallback was.

## Qt modules used and why

- **Qt Quick / Qt Qml / Qt Quick Controls 2** (both apps) — the panel UI and
  the control-center shell.
- **Qt Quick 3D**, **Qt Quick 3D Particles**, **Qt Quick Timeline**
  (offboard) — the 3D factory scene, particle effects, and the authored
  camera fly-to keyframe animation.
- **Qt Quick 3D Physics**, **Qt Graphs** (offboard, guarded/optional) — a
  physics-driven interaction and the 2D/3D charts, when the local Qt install
  provides them; both are GPL/commercial-only, which is why this app's whole
  directory is GPL-3.0-or-later regardless.
- **Qt StateMachine** (both apps) — the machine lifecycle (onboard) and the
  maintenance workflow / automated demo conductor (offboard).
- Neither app uses `QtNetwork`/`QtSql` on the onboard side, per this
  repository's onboard/offboard convention; the offboard app models
  industrial connectivity with an in-process simulator rather than requiring
  a live MQTT broker.

## Process note

As with every other industry in this repository, these are technology
showcases, not certified or production-ready software. Where this repository
references a software life-cycle process generically, it uses
**ISO/IEC/IEEE 12207**.
