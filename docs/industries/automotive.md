# Automotive

Two example applications show the same "onboard/offboard" split that shows
up across most connected-vehicle products: software that runs *in* the
vehicle, and software that runs *about* many vehicles.

## Onboard vs. offboard, concretely

- **Onboard — `industries/automotive/onboard-instrument-cluster/`.** This is
  what would run on the vehicle's own compute unit, driving the digital
  instrument cluster in front of the driver: speedometer, rev counter, gear
  indicator, turn signals, and an ADAS-style following-distance strip. It has
  no network connection and persists nothing — every session starts fresh,
  exactly like a real cluster boots up each time the vehicle is powered on.
  The whole point is a low-latency, GPU-accelerated HMI reacting to a live
  (here, simulated) sensor/CAN feed.
- **Offboard — `industries/automotive/offboard-fleet-dashboard/`.** This is
  what would run in a fleet operator's control room or cloud back office: a
  table of many vehicles at once (id, location placeholder, speed, fuel,
  fault codes), a trend view for whichever vehicle is selected, and a SQLite
  history so past telemetry can be queried after the fact. It exists
  precisely because no single vehicle's onboard computer has visibility
  across the whole fleet, or a reason to keep long-term history.

## Why the underlying simulation matters

Both apps generate believable, correlated data instead of random noise:
`DriveCycleSimulator` (onboard) drives speed through a repeating
accelerate/cruise/brake/stop/reverse cycle with first-order lag, and derives
gear, rpm, fuel drain, and following-distance warnings from that single speed
signal — so a viewer sees the gear shift up as speed rises and the ADAS strip
collapse exactly when the cluster is braking, the way a real drivetrain
behaves. `FleetVehicleSimulator` (offboard) reuses the same idea per vehicle,
phase-shifted so the fleet does not move in lockstep.

## Qt modules used, and why they matter here

| App | Modules | Why |
| --- | --- | --- |
| Onboard instrument cluster | Qt Quick, Qt Quick `Canvas`, Qt Qml | A scene-graph-based, GPU-accelerated UI is what keeps an embedded cluster's frame rate smooth on constrained automotive silicon; `Canvas` gives hand-painted, resolution-independent gauges without image assets or a charting dependency. |
| Offboard fleet dashboard | Qt Widgets, QtSql | A desktop/back-office control room favours a traditional widget/model-view stack; QtSql (SQLite) provides the durable, queryable telemetry archive an operations team needs, which an onboard HMI has no reason to carry. |

Both logic layers (`DriveCycleSimulator`, `FleetVehicleSimulator`,
`TelemetryHistoryStore`) are plain C++ with no Qt GUI dependency, unit tested
with QTest under `tests/industries/automotive/`. In a real ISO 26262 /
ASPICE-governed development, that separation is what makes the drivetrain
and telemetry logic traceable to documented requirements independently of
whatever UI framework happens to sit on top.
