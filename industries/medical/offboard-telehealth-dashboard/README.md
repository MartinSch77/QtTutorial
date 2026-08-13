# Offboard Telehealth Dashboard (Medical)

A control-room style telehealth dashboard: software that runs away from any
one patient's bedside, aggregating vitals from several remote monitors, with
persistence and an alert list — the "offboard" counterpart to
`onboard-patient-monitor`.

## Design reference

The dark cards with severity-coloured borders, boxed numeric vitals labelled
with small clinical icons, and a blue trend trace are a stylistic homage to
the general visual language of bedside/telehealth patient monitoring
displays such as the **Philips IntelliVue MX-series** / **GE Healthcare
CARESCAPE** family (the same genre referenced by the onboard companion app).
This is style inspiration only, not a clone: no Philips or GE logo,
wordmark, trademark, or exact copyrighted screen layout is reproduced, and
every icon on screen (`IconPainter`) is drawn procedurally with `QPainter`
paths at paint time rather than loaded from an image or icon-font asset.

## What it demonstrates

- `PatientVitalsSimulator` stands in for a telehealth ingestion service: it
  produces a deterministic, per-patient, phase-shifted vitals feed with
  plausible respiratory-driven variability, plus **two independent,
  clinically-motivated scenarios** so vitals correlate the way a real
  patient's would rather than drifting independently:
  - patient index 0 periodically runs a short **desaturation event** (heart
    rate up, SpO2 down, respiration rate up too — low oxygen plausibly
    correlates with faster, more laboured breathing) so the alert list has
    something real to surface;
  - patient index 1 periodically runs a longer, offset **fever episode**
    (temperature up, with a milder correlated rise in heart rate and
    respiration rate).
- `PatientListModel`, a `QAbstractListModel`, aggregates several patients,
  each carrying a rolling heart-rate trend buffer.
- `PatientOverviewWidget`, a **multi-patient overview grid**: one small
  `PatientCardWidget` per patient, each showing name, heart rate, SpO2 and
  blood pressure labelled with a procedurally-drawn clinical icon
  (`IconPainter` — a heartbeat blip, an SpO2 droplet, a blood-pressure cuff,
  and an alarm bell, all built from `QPainter`/`QPainterPath` line, cubic
  bezier, and arc segments, no external icon asset) and a severity-coloured
  border. Clicking a card selects that patient for the trend chart below.
- `SparklineDelegate` paints each detailed list row's vitals summary plus a
  small `QPainter`-drawn heart-rate sparkline directly in the list view (no
  charting library — Qt Charts/Graphs are not MIT licensed).
- `TrendChartWidget`, a **trend chart** for the currently selected patient:
  a `QPainter`-drawn line chart (the same hand-painted-line approach as
  `SparklineDelegate`, just larger and with axis labels/gridlines) of that
  patient's heart rate over the last few minutes, sourced from
  `VitalsHistoryStore`.
- An alert list, populated from `PatientVitalsSimulator::classify`, sorted so
  the most severe patients surface first.
- `VitalsHistoryStore` persists every patient's vitals to a SQLite database
  via `QtSql`, demonstrating the kind of durable, queryable archive a
  telehealth back office needs — this is also what backs the trend chart.
  The shipped app uses an in-memory database for a self-contained demo.

## Qt modules/APIs exercised

- **Qt Widgets** (`QListView` + a custom `QStyledItemDelegate`, `QListWidget`)
  — appropriate for a desktop/back-office control room application.
- **QtSql** (`QSqlDatabase`, `QSqlQuery`) for the vitals history archive.
- **`QPainter`** for the sparkline, avoiding a charting dependency.
- **`QTimer`** simulating a periodic multi-patient telemetry push.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target offboard_telehealth_dashboard
./build/industries/medical/offboard-telehealth-dashboard/offboard_telehealth_dashboard
```

## Tests

See `tests/industries/medical/offboard-telehealth-dashboard/`, which covers
`PatientVitalsSimulator`'s per-patient variability and severity
classification — including that the desaturation event and fever episode
each correlate the right vitals together — and `VitalsHistoryStore`'s
insert/query behaviour against an in-memory SQLite database — no UI
dependency. `PatientOverviewWidget`/`TrendChartWidget`/`IconPainter` are pure
UI/painting code (consistent with `SparklineDelegate` and `MainWindow`
already in this app) and are exercised by running the app rather than by
QTest.
