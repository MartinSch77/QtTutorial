# Two-Wheelers

Two example applications show the same "onboard/offboard" split that shows
up across most connected-vehicle products, this time for motorcycles and
bike-share/delivery fleets of e-bikes and e-scooters: software that runs *on*
a single two-wheeler, and software that runs *about* many of them.

## Onboard vs. offboard, concretely

- **Onboard — `industries/two-wheelers/onboard-rider-dashboard/`.** This is
  what would run on a motorcycle's own dash unit, in front of the rider:
  speedometer, rev counter, gear indicator, and — the distinctive two-wheeler
  HMI element — a lean-angle horizon indicator, plus a small front/rear
  tyre-pressure-and-temperature readout. It has no network connection and
  persists nothing — every session starts fresh, exactly like a real dash
  boots up each time the bike is switched on. The whole point is a
  low-latency, GPU-accelerated HMI reacting to a live (here, simulated) IMU
  and wheel-speed feed.
- **Offboard — `industries/two-wheelers/offboard-fleet-dashboard/`.** This is
  what would run in a bike-share or delivery operator's control room: a table
  of many vehicles at once (id, location placeholder, speed, battery level,
  rider status), a trend view for whichever vehicle is selected, and a SQLite
  history so past telemetry can be queried after the fact. It exists
  precisely because no single vehicle's onboard unit has visibility across
  the whole fleet, or a reason to keep long-term history.

## Why the underlying simulation matters

Both apps generate believable, correlated data instead of random noise.
`RideCycleSimulator` (onboard) drives speed through a repeating
accelerate/cruise/brake/corner/accelerate/stop cycle with first-order lag, and
derives gear and rpm from that single speed signal — but the key coupling is
lean angle: it is driven by the *cornering phase* of the cycle and the
current speed within it, not independent noise, so the horizon indicator only
tilts while the bike is actually simulated to be turning, and tilts further
the faster it is going through that corner. Tyre temperature drifts toward a
speed-dependent equilibrium (sustained speed heats the tyres, with the rear
running hotter than the front), and tyre pressure is derived directly from
that temperature, the way a real tyre's pressure rises as it warms up.
`FleetVehicleSimulator` (offboard) reuses the same idea per vehicle,
phase-shifted so the fleet does not move in lockstep, with battery level
draining and wrapping into deterministic charging stops, and periodic
maintenance windows, rather than arbitrary randomness.

## Qt modules used, and why they matter here

| App | Modules | Why |
| --- | --- | --- |
| Onboard rider dashboard | Qt Quick, Qt Quick `Canvas`, Qt Qml | A scene-graph-based, GPU-accelerated UI is what keeps an embedded dash's frame rate smooth on constrained silicon; `Canvas` gives hand-painted, resolution-independent gauges and a rotating lean-angle horizon without image assets or a charting dependency. |
| Offboard fleet dashboard | Qt Widgets, QtSql | A desktop/back-office control room favours a traditional widget/model-view stack; QtSql (SQLite) provides the durable, queryable telemetry archive an operations team needs, which an onboard HMI has no reason to carry. |

Both logic layers (`RideCycleSimulator`, `FleetVehicleSimulator`,
`TelemetryHistoryStore`) are plain C++ with no Qt GUI dependency, unit tested
with QTest under `tests/industries/two-wheelers/`. In a real
ISO/IEC/IEEE 12207-governed development, that separation is what makes the
ride-cycle and telemetry logic traceable to documented requirements
independently of whatever UI framework happens to sit on top.
