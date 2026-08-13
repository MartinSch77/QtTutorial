# Mining

Two example applications show the same "onboard/offboard" split that shows
up across most heavy-equipment products: software that runs *in* a single
piece of equipment, and software that runs *about* many pieces of equipment
at once.

## Onboard vs. offboard, concretely

- **Onboard — `industries/mining/onboard-haul-truck-console/`.** This is what
  would run on a large mining haul truck's own cab-mounted compute unit: a
  payload gauge in tonnes with an overload warning, engine and retarder
  temperature gauges, a six-tyre pressure grid, and a prominent haul-cycle
  banner showing which of the four phases — Loading, Hauling, Dumping,
  Returning — the truck is currently in. It has no network connection and
  persists nothing — every session starts fresh, exactly like a real cab
  display boots up each time the truck is powered on. The whole point is a
  low-latency HMI reacting to a live (here, simulated) haul-cycle state
  machine.
- **Offboard — `industries/mining/offboard-pit-operations-dashboard/`.** This
  is what would run in a pit control room or mine-wide back office: a table
  of several trucks at once (id, haul state, payload, location placeholder),
  a production trend view of cumulative tonnes hauled for whichever truck is
  selected, and a SQLite history so past haul-cycle activity can be queried
  after the fact. It exists precisely because no single truck's onboard
  computer has visibility across the whole fleet, or a reason to keep
  long-term production history.

## Why the underlying simulation matters

Both apps are driven by an explicit small state machine over the four
haul-cycle phases (Loading, Hauling, Dumping, Returning) with plausible dwell
times (30 s / 90 s / 15 s / 60 s), rather than random noise. `HaulCycleSimulator`
(onboard) derives every other signal from that same state/time: payload ramps
up during Loading (briefly overfilling before being trimmed back to rated
capacity, which is what triggers the overload warning), holds at rated
capacity through Hauling, ramps back to zero during Dumping, and sits at zero
while Returning empty; engine temperature climbs while loading and hauling a
full load and eases off while returning empty; retarder temperature climbs
specifically while descending the haul road into the dump point and through
dumping, then cools on the way back; and tyre pressure creeps up slightly with
payload rather than varying independently. `HaulFleetSimulator` (offboard)
reuses the same four-stage cycle per truck, phase-shifted so the fleet does
not move in lockstep, and derives cumulative tonnes hauled as a pure function
of how many full cycles a truck has completed.

## Qt modules used, and why they matter here

| App | Modules | Why |
| --- | --- | --- |
| Onboard haul truck console | Qt Quick, Qt Quick `Canvas`, Qt Qml | A scene-graph-based, GPU-accelerated UI is what keeps a cab-mounted console's frame rate smooth on constrained embedded hardware; `Canvas` gives hand-painted, resolution-independent gauges without image assets or a charting dependency. |
| Offboard pit operations dashboard | Qt Widgets, QtSql | A desktop/back-office control room favours a traditional widget/model-view stack; QtSql (SQLite) provides the durable, queryable haul-cycle archive an operations team needs, which an onboard console has no reason to carry. |

Both logic layers (`HaulCycleSimulator`, `HaulFleetSimulator`,
`HaulHistoryStore`) are plain C++ with no Qt GUI dependency, unit tested with
QTest under `tests/industries/mining/`. In a real ISO/IEC/IEEE
12207-governed development, that separation is what makes the haul-cycle
logic traceable to documented requirements independently of whatever UI
framework happens to sit on top.
