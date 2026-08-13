# Onboard Patient Monitor (Medical)

A kiosk-style Qt Quick bedside patient monitor: representative of software
that would run embedded on the monitoring device itself, at the point of
care — no network, no persistence, just a real-time animated HMI.

## What it demonstrates

- A live ECG-like waveform (`EcgCanvas.qml`, a `Canvas` item) driven by
  `EcgWaveformGenerator`, which builds a simplified PQRST cardiac-cycle shape
  from a handful of Gaussian bumps rather than random noise, at heart-rate
  dependent timing — the waveform visibly speeds up or slows down with heart
  rate.
- Heart rate/SpO2/blood-pressure numeric tiles (`VitalTile.qml`) fed by
  `VitalsSimulator`, which layers a plausible respiratory-driven wobble on a
  baseline heart rate and periodically runs a short, deterministic
  "deterioration episode" (heart rate up, SpO2 down, and back) so the alarm
  system has something believable to react to.
- An alarm banner driven by `AlarmStateMachine`, a small
  Normal/Warning/Critical state machine with **hysteresis**: the thresholds
  needed to enter a more severe level are looser than the thresholds needed
  to leave it, so a vitals value sitting right on a boundary does not make
  the banner flicker.
- `PatientMonitor` is a thin `QObject`/`QML_ELEMENT` façade that steps the
  simulation on a `QTimer` and republishes state via `Q_PROPERTY`; all of the
  physiological model and alarm logic lives in plain C++ classes with no Qt
  GUI dependency, so they are unit tested headlessly.

## Qt modules/APIs exercised

- **Qt Quick / QML** for a GPU-accelerated, low-footprint HMI suitable for an
  embedded bedside device.
- **Qt Quick `Canvas`** for the ECG trace, avoiding any charting library
  (Qt Charts/Graphs are not MIT licensed).
- **`QML_ELEMENT`** / `qt_add_qml_module` for direct C++-to-QML type
  registration.
- **`QTimer`** driving a fixed-step simulation, standing in for a real sensor
  front end (ECG lead, pulse oximeter, NIBP cuff).

Where a process standard is relevant to this kind of software, see
`docs/industries/medical.md`, which references ISO 12207 (software life
cycle processes) rather than any specific safety-classification standard.

## Build & run

```sh
cmake -S . -B build -DQTTUTORIAL_BUILD_INDUSTRIES=ON
cmake --build build --target onboard_patient_monitor
./build/industries/medical/onboard-patient-monitor/onboard_patient_monitor
```

## Tests

See `tests/industries/medical/onboard-patient-monitor/`, which covers the ECG
waveform shape, the vitals model, and — most importantly — the alarm state
machine's hysteresis behaviour (no flicker on borderline values), all without
any Qt GUI dependency.
