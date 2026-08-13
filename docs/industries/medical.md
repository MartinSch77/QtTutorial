# Medical

Two example applications show the same "onboard/offboard" split seen across
connected medical devices: software that runs *at the point of care*, and
software that runs *away from it*.

## Onboard vs. offboard, concretely

- **Onboard — `industries/medical/onboard-patient-monitor/`.** This is what
  would run embedded in a bedside monitor itself: a live ECG-like waveform,
  heart-rate/SpO2/blood-pressure tiles, and an alarm banner, all reacting in
  real time to a simulated sensor front end. It has no network connection
  and persists nothing between sessions — a bedside monitor's job is to
  display *now*, reliably and at a real refresh rate, not to archive.
- **Offboard — `industries/medical/offboard-telehealth-dashboard/`.** This is
  what would run in a telehealth back office: several simulated patients
  aggregated into one list, each with current vitals and a short trend
  sparkline, an alert list ordered by severity, and a SQLite-backed history
  for one patient's vitals over time. It exists because no single bedside
  monitor has visibility across an entire patient panel, or a reason to keep
  long-term history for reporting.

## Why the underlying simulation matters

The onboard app's `EcgWaveformGenerator` builds a simplified PQRST cardiac
waveform shape from a handful of Gaussian bumps rather than random noise, at
heart-rate-dependent timing, so the trace visibly speeds up or slows down
with heart rate the way a real ECG does. `VitalsSimulator` layers a plausible
respiratory-driven wobble onto baseline heart rate and periodically runs a
short, deterministic deterioration episode (heart rate up, SpO2 down, and
back) — believable structure, not noise — so `AlarmStateMachine` has
something real to react to. That state machine is a small
Normal/Warning/Critical machine with **hysteresis**: the thresholds needed to
enter a more severe level are looser than those needed to leave it, so a
vitals value sitting right on a boundary does not make the alarm banner
flicker. The offboard app's `PatientVitalsSimulator` reuses the same idea
per patient, phase-shifted, with one patient periodically deteriorating so
the alert list has something to surface.

## Qt modules used, and why they matter here

| App | Modules | Why |
| --- | --- | --- |
| Onboard patient monitor | Qt Quick, Qt Quick `Canvas`, Qt Qml | A GPU-accelerated scene graph keeps the waveform animation smooth on constrained embedded medical hardware; `Canvas` draws the ECG trace without an image asset or charting dependency. |
| Offboard telehealth dashboard | Qt Widgets, QtSql | A back-office dashboard favours a traditional widget/model-view stack (a custom `QStyledItemDelegate` paints each patient's sparkline); QtSql (SQLite) provides the durable, queryable vitals archive a telehealth service needs, which a bedside monitor has no reason to carry. |

Both logic layers (`EcgWaveformGenerator`, `VitalsSimulator`,
`AlarmStateMachine`, `PatientVitalsSimulator`, `VitalsHistoryStore`) are plain
C++ with no Qt GUI dependency, unit tested with QTest under
`tests/industries/medical/`. Where a process standard is relevant to this
kind of software, this repository references **ISO 12207** (software life
cycle processes) rather than any device-safety-classification standard —
separating the physiological/alarm logic from the UI is what makes that
logic independently reviewable and testable, which is the underlying point
of any such process standard.
