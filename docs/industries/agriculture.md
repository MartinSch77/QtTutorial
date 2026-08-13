# Agriculture

Two example applications show the same "onboard/offboard" split that shows
up across most connected-machinery products: software that runs *in* the
tractor or combine, and software that runs *about* many fields and machines.

## Onboard vs. offboard, concretely

- **Onboard — `industries/agriculture/onboard-tractor-console/`.** This is
  what would run on the tractor or combine's own in-cab display, driving a
  GPS-guided steering-assist console: a cross-track error scale showing how
  far the machine has drifted from the planted row line, implement status
  (working depth and engaged/raised state), and fuel-level and engine-load
  gauges, plus a field-coverage arc showing how much of the current pass has
  been completed. It has no network connection and persists nothing — every
  session starts fresh, exactly like a real in-cab console boots up each time
  the machine is powered on. The whole point is a low-latency HMI reacting to
  a live (here, simulated) GPS/implement-control feed.
- **Offboard — `industries/agriculture/offboard-farm-operations-dashboard/`.**
  This is what would run in a farm manager's office or cloud back office: a
  table of many fields/vehicles at once (vehicle id, field id, pass coverage,
  status), a field map for whichever field is selected, and a SQLite
  operation history so past pass-completion progress can be queried after the
  fact. It exists precisely because no single machine's in-cab console has
  visibility across the whole operation, or a reason to keep long-term
  history.

## Why the underlying simulation matters

Both apps generate believable, correlated data instead of random noise:
`FieldPassSimulator` (onboard) advances field coverage deterministically with
distance travelled along a fixed-length pass, and derives cross-track error
from a smooth, bounded function of that same distance signal (a sum of two
sinusoids of different periods, standing in for ongoing steering-assist
correction rather than independent per-tick noise), while engine load and
fuel burn are both derived from whether the implement is currently engaged —
so a viewer sees the coverage arc fill steadily, the implement disengage in
the short turn zones at each row end, and engine load/fuel burn rise and fall
exactly when the console shows the implement engaged, the way a real machine
behaves. `FieldOperationSimulator` (offboard) reuses the same idea per field,
phase-shifted so fields do not progress in lockstep, with an added
idle/downtime window at the start of each pass.

## Qt modules used, and why they matter here

| App | Modules | Why |
| --- | --- | --- |
| Onboard tractor console | Qt Quick, Qt Quick `Canvas`, Qt Qml | A scene-graph-based, GPU-accelerated UI is what keeps an in-cab console's frame rate smooth on constrained machine-control silicon; `Canvas` gives hand-painted, resolution-independent gauges and scales without image assets or a charting dependency. |
| Offboard farm operations dashboard | Qt Widgets, QtSql | A desktop/back-office operations view favours a traditional widget/model-view stack; QtSql (SQLite) provides the durable, queryable operation-history archive a farm-management team needs, which an in-cab console has no reason to carry. |

Both logic layers (`FieldPassSimulator`, `FieldOperationSimulator`,
`OperationHistoryStore`) are plain C++ with no Qt GUI dependency, unit tested
with QTest under `tests/industries/agriculture/`. In a real ISO/IEC/IEEE
12207-governed development, that separation is what makes the guidance and
pass-progress logic traceable to documented requirements independently of
whatever UI framework happens to sit on top.
