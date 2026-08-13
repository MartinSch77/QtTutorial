# Offboard Telehealth Dashboard (Medical)

A control-room style telehealth dashboard: software that runs away from any
one patient's bedside, aggregating vitals from several remote monitors, with
persistence and an alert list — the "offboard" counterpart to
`onboard-patient-monitor`.

## What it demonstrates

- `PatientVitalsSimulator` stands in for a telehealth ingestion service: it
  produces a deterministic, per-patient, phase-shifted vitals feed with
  plausible respiratory-driven variability, and periodically runs a short
  deterioration episode on one patient so the alert list has something real
  to surface (not independent random noise).
- `PatientListModel`, a `QAbstractListModel`, aggregates several patients,
  each carrying a rolling heart-rate trend buffer.
- `SparklineDelegate` paints each row's vitals summary plus a small
  `QPainter`-drawn heart-rate sparkline directly in the list view (no
  charting library — Qt Charts/Graphs are not MIT licensed).
- An alert list, populated from `PatientVitalsSimulator::classify`, sorted so
  the most severe patients surface first.
- `VitalsHistoryStore` persists one patient's vitals to a SQLite database via
  `QtSql`, demonstrating the kind of durable, queryable archive a telehealth
  back office needs. The shipped app uses an in-memory database for a
  self-contained demo.

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
classification and `VitalsHistoryStore`'s insert/query behaviour against an
in-memory SQLite database — no UI dependency.
