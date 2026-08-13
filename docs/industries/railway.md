# Railway

Two example applications, illustrating the onboard/offboard split for a
passenger railway: one runs *on* the train, one runs *away from* it.

## Onboard cab display

`industries/railway/onboard-cab-display/` is what would be built into the
driving desk itself — a fullscreen Qt Quick display with no network
dependency and no persistence, showing only this train's current state.
A `TrainSimulator` advances a train along a `RouteProfile` (a sequence of
speed-restriction segments and station stops), so current speed, permitted
speed, and the driving-advice margin bar are all geometrically consistent
with one underlying model rather than picked independently. The permitted
speed is derived from an ETCS-style braking-curve function
(`v = sqrt(target² + 2·a·d)`), the signal aspect and door-status indicators
reflect the same simulated state. As with the industrial onboard panel,
there is nothing to send anywhere: the cab display's job is showing *this
train, right now*.

## Offboard fleet operations centre

`industries/railway/offboard-fleet-ops-center/` is the control-room
counterpart — a fleet dashboard that would run in an operations centre or
on a cloud back end, aggregating several trains' positions and schedule
adherence at once. Every simulated train's position, speed and delay is
persisted to a SQLite-backed run history via `QtSql` (schema, parameterized
statements, range queries, and a fleet-wide average-delay query for
punctuality reporting), shown live in a table, with a hand-rolled
`QPainter` delay trend for whichever train is selected. Where the cab
display only ever needs "is this train, right now, running safely", the
operations centre exists to answer "how is the whole fleet doing, and how
has it been doing".

## The onboard/offboard distinction, concretely

| | Onboard cab display | Offboard fleet ops centre |
|---|---|---|
| Runs on | the driving desk itself | control room / cloud server |
| Network | none | conceptually multi-train (simulated here) |
| Persistence | none — live state only | SQLite run history, queryable by time range |
| UI | fullscreen kiosk, Qt Quick | windowed dashboard, Qt Widgets |
| Core question answered | "Am I safe, right now?" | "How punctual is the fleet, over time?" |

## Qt modules used and why

- **Qt Quick / QML** (onboard) — the cab display needs to be legible at a
  glance and update smoothly at speed; declarative bindings keep the
  speedometer, margin bar and indicators in sync with the simulated train
  state without manual repaint code.
- **Qt Core** (both) — `QTimer`-driven simulation loops and
  `Q_PROPERTY`/signal plumbing connect the testable logic layer
  (`RouteProfile`, `BrakingCurve`, `TrainSimulator` on the onboard side;
  `FleetSimulator`, `DelayCalculator` on the offboard side) to whichever UI
  sits on top.
- **Qt Widgets** (offboard) — a data-dense, resizable fleet table and
  trend view for a desk-bound dispatcher is a natural fit for Widgets.
- **Qt SQL** (offboard) — punctuality reporting is fundamentally a
  time-series persistence and aggregation problem; `QSQLITE` with
  parameterized statements demonstrates that without requiring a running
  database server just to build and test the tutorial.

Both apps deliberately avoid Qt Charts and Qt Graphs (GPL/commercial-only
for this tutorial's purposes) in favour of hand-drawn `QPainter` trends and
plain QML shapes, keeping the whole example MIT-licensed.

The braking-curve and route-consistency logic underneath the cab display is
exactly the sort of safety-relevant software life-cycle artifact **ISO
12207** describes generically — specified, implemented and verified before
it influences a moving vehicle — even though this tutorial's simulated
route carries no real operational authority.
