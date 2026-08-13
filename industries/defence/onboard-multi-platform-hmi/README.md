# Onboard Multi-Platform HMI

A fullscreen, kiosk-style embedded HMI representative of software that runs
*on board* several different defence platform types (a vehicle crew station,
a sensor-track operator console, a drone ground-control display, and an
AR/HUD-style soldier display), switched between via a tab bar rather than
shipped as four separate apps — the same onboard/offboard split every other
industry in this repository uses, applied here across several platform
types instead of one.

## Safety framing — read this first

**This is passive display/visualization software with simulated data. It
has no weapons, targeting, or fire-control capability, by design.**

- The **Sensor Track Display** panel is a radar-plot-style visualization of
  simulated tracked objects (position, heading, a generic classification
  label such as "Fixed-wing" or "Unknown contact"). It carries no weapons
  data and computes no aim/intercept solution — it is exactly the kind of
  passive situational-awareness screen an air-defence or maritime sensor
  operator watches, showing *what is out there*, never *how to engage it*.
  This panel is the one place a "rocket tracing tool" requirement could
  legitimately live in a Qt showcase, and it is implemented strictly as a
  track display for that reason.
- The **Drone Object Detection** panel visualizes what a detection
  pipeline's *output* looks like drawn over a camera feed (simulated boxes,
  class labels, confidence percentages over a simulated animated
  background). There is no real camera, no real ML model, and no
  targeting/aim computation anywhere in it.
- The **AR HUD Overlay** panel is a simulated first-person heads-up display
  (compass heading, waypoint markers, teammate position indicators) — an
  informational overlay, not a weapon sight.
- The **Vehicle Crew Display** panel is a status board (heading, speed,
  fuel/power, subsystem health) with no control inputs exposed anywhere.
- The **Comms & Data Link** panel is a per-subsystem antenna/signal readout
  plus a list of simulated off-board tracks received over a tactical data
  link, each shown with its own freshness/staleness. It only ever reports
  signal quality and data age — never anything about the tracked object
  beyond position/heading/domain classification.

Deliberately omitted: any weapons guidance/targeting algorithm, fire-control
computation, munitions trajectory/aiming logic, or anything that could
function as targeting assistance. If a future extension of this example is
tempted to add such a thing, don't — it does not belong in a public,
MIT-licensed tutorial repository.

## What it demonstrates

- **A pure, testable simulation core.** `TrackMotionModel` (radar tracks),
  `VehicleStatusSimulator` (heading/speed/fuel), `DetectionSimulator`
  (bounding-box motion) and `HudSimulator` (compass/waypoint/teammate
  bearings) are all plain C++23 classes in `defence_onboard_lib`, with no Qt
  Quick dependency, so their motion models are unit-testable without a
  running QML engine.
- **The state-machine-driven health-grid pattern**, applied to a vehicle
  crew station: `SubsystemHealthMonitor` classifies a value into
  Nominal/Caution/Critical and emits a signal only on a classification
  *change*; `SubsystemHealthMachine` wires those edge-triggered signals into
  a real `QStateMachine` (three `QState`s) via `QState::addTransition`. Four
  independent machines (Propulsion/Power/Comms/Sensors) drive the health
  grid shown on the crew display.
- **Smooth, non-teleporting motion.** Tracks turn at a fixed rate and bounce
  back toward the center of the surveillance area when they reach its edge;
  detection boxes bounce within the simulated video frame; the vehicle's
  heading eases toward a slowly-cycling target heading rather than jumping.
- **QML `Canvas` instrumentation.** The radar plot (range rings, track dots,
  heading vectors) and the HUD compass strip are hand-painted on `Canvas`,
  not static images, and not Qt Charts/Graphs (both GPL/commercial-only).
- **An onboard characteristic, by omission.** No `QtNetwork`, no `QtSql`, no
  persistence — everything is generated in-process by a single
  `QTimer`-driven facade (`PlatformSimulator`), consistent with embedded,
  closed, real-time onboard software everywhere else in this repository.

## Qt modules used

- **Qt Quick / Qt Qml** for the tabbed, translucent-overlay-style panels —
  a natural fit for AR-HUD-style and camera-overlay-style displays that
  Widgets is a poor match for.
- **Qt Quick Controls 2** for the tab bar switching between the four panels.
- **Qt StateMachine** for the subsystem health state machines.
- **Qt Core** (`QTimer`, `QObject` signals/properties) to drive the
  simulation loop and publish it to QML.

## Process note

Real defence situational-awareness software of this kind is often governed
by domain-specific standards analogous to safety/airworthiness standards
such as DO-178C for airborne systems. This repository does not claim any
such compliance — it is a technology showcase, not a certified artifact.
Where a life-cycle process is referenced generically elsewhere in this
repository, it is **ISO/IEC/IEEE 12207**.

## Build & run

From the repository root:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --target onboard_multi_platform_hmi
./build/industries/defence/onboard-multi-platform-hmi/onboard_multi_platform_hmi
```

## Tests

`TrackMotionModel`, `VehicleStatusSimulator`, `SubsystemHealthMachine`,
`DetectionSimulator` and `DataLinkTrackModel` are unit-tested with QTest
under `tests/industries/defence/onboard-multi-platform-hmi/`.
