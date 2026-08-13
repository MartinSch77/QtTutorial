# Industrial Automation

Two example applications, illustrating the onboard/offboard split for a
process plant: one runs *on* the equipment, one runs *away from* it.

## Onboard HMI panel

`industries/industrial-automation/onboard-hmi-panel/` is what would run
embedded on the operator terminal bolted to a skid or control cabinet — a
fullscreen Qt Quick mimic-diagram HMI with no network dependency and no
persistence. It shows a single unit's live state: a tank whose level
integrates inflow minus outflow, a pump whose lifecycle (Idle → Running →
Fault, only clearable by explicit reset) is modelled with `QStateMachine`
including a guarded transition, and a severity-coloured scrolling alarm
banner. Everything on screen is generated in-process by a `QTimer`-driven
simulation; there is nothing to query or store because there is nothing
"back home" to send it to — this process unit *is* the plant floor.

## Offboard plant SCADA

`industries/industrial-automation/offboard-plant-scada/` is the control-room
counterpart: a historian dashboard that would run in the SCADA server room
or the cloud, aggregating readings *as if* collected from several onboard
units' sensors. It persists every simulated tag reading to a SQLite-backed
historian via `QtSql` (a real schema, parameterized `INSERT`/`SELECT`
statements, range queries), draws a hand-rolled `QPainter` trend for
whichever tag is selected, and keeps an alarm history log. Where the onboard
panel only ever knows "now", the offboard dashboard's whole reason to exist
is remembering and querying "then".

## The onboard/offboard distinction, concretely

| | Onboard HMI panel | Offboard plant SCADA |
|---|---|---|
| Runs on | the equipment/cabinet itself | control room / cloud server |
| Network | none | conceptually multi-unit (simulated here) |
| Persistence | none — live state only | SQLite historian, queryable by time range |
| UI | fullscreen kiosk, Qt Quick | windowed dashboard, Qt Widgets |
| Failure mode shown | pump Fault state, tank alarms | alarm history, trend over time |

## Qt modules used and why

- **Qt Quick / QML** (onboard) — a touch-first, animatable kiosk UI is the
  natural fit for equipment-mounted HMIs; declarative bindings keep the
  mimic diagram in sync with process state with no manual repaint logic.
- **Qt State Machine** (onboard) — a pump's Idle/Running/Fault lifecycle
  with a guarded start transition is exactly the kind of explicit,
  auditable state model process-safety reviews expect; `QStateMachine`
  makes the transition graph inspectable rather than buried in `if`/`else`.
- **Qt Widgets** (offboard) — a data-dense, resizable, keyboard-and-mouse
  dashboard for a desk-bound operator is still Widgets' sweet spot.
- **Qt SQL** (offboard) — process historians are fundamentally a
  time-series persistence problem; `QSQLITE` with parameterized queries
  demonstrates the pattern without needing a client/server database
  running for the tutorial to build and test.
- **Qt Core** (both) — `QTimer`-driven simulation loops and
  `Q_PROPERTY`/signal plumbing tie the testable logic layer to whichever UI
  toolkit sits on top of it.

Both apps deliberately avoid Qt Charts and Qt Graphs (GPL/commercial-only
for this tutorial's purposes) in favour of hand-drawn `QPainter` trends and
plain QML shapes, keeping the whole example MIT-licensed.

Both apps' process-adjacent logic (signal handling, alarm classification,
state transitions) is the kind of software life-cycle activity **ISO 12207**
describes generically — verification and validation of software components
before they influence a physical process — even though this tutorial's
simulated plant carries no real safety obligation.
