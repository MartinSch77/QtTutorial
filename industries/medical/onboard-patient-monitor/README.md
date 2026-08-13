# Onboard Patient Monitor (Medical)

A kiosk-style Qt Quick bedside patient monitor: representative of software
that would run embedded on the monitoring device itself, at the point of
care — no network, no persistence, just a real-time animated HMI.

## Design reference

The dark background, large colour-coded waveform traces (ECG green,
respiration yellow), boxed numeric vitals with an accent border, and an
urgency-coloured alarm banner are a stylistic homage to the general visual
language of bedside patient monitors such as the **Philips IntelliVue
MX-series** / **GE Healthcare CARESCAPE** family. This is genre/style
inspiration only, not a clone: no Philips or GE logo, wordmark, trademark, or
exact copyrighted screen layout is reproduced anywhere in this app, and every
icon and waveform on screen is drawn procedurally (via QML `Canvas` paths) at
paint time rather than loaded from an image or icon-font asset.

## What it demonstrates

- A live ECG-like waveform (`EcgCanvas.qml`, a `Canvas` item) driven by
  `EcgWaveformGenerator`, which builds a simplified PQRST cardiac-cycle shape
  from a handful of Gaussian bumps rather than random noise, at heart-rate
  dependent timing — the waveform visibly speeds up or slows down with heart
  rate.
- A live respiration waveform (`RespirationCanvas.qml`, also `Canvas`) driven
  by `RespirationWaveformGenerator`, which shapes a fast inspiratory rise and
  slower expiratory fall from two cosine ramps, at a rate tied to the
  simulated respiration rate.
- Heart rate/SpO2/blood-pressure/respiration-and-temperature numeric tiles
  (`VitalTile.qml`), each labelled with a small procedurally-drawn clinical
  icon (`ClinicalIcon.qml` — a heartbeat blip, an SpO2 droplet, a
  blood-pressure cuff, and a respiration wave, all built from `Canvas` line
  and bezier-curve paths, no external icon asset), fed by `VitalsSimulator`.
- `VitalsSimulator` layers a plausible respiratory-driven wobble on a
  baseline heart rate, plus **two independent, clinically-motivated
  scenarios** so vitals correlate the way a real patient's would rather than
  drifting independently:
  - a short **desaturation event** (heart rate up, SpO2 down, respiration
    rate up — low oxygen plausibly correlates with faster, more laboured
    breathing) on a 90 second cycle;
  - a longer **fever episode** (temperature up, with a milder correlated
    rise in heart rate and respiration rate — a low-grade
    tachycardia/tachypnoea accompanying a fever) on an offset 150 second
    cycle, independent of the desaturation event.
- An alarm banner driven by `AlarmStateMachine`, a small
  Normal/Caution/Critical state machine with **hysteresis**: the thresholds
  needed to enter a more severe level are looser than the thresholds needed
  to leave it, so a vitals value sitting right on a boundary does not make
  the banner flicker. The banner is colour-coded (green/amber/red) and
  carries a procedurally-drawn alarm-bell icon, and pulses when critical.
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
and respiration waveform shapes, the vitals model — including that the
desaturation event and fever episode correlate the right vitals together —
and — most importantly — the alarm state machine's hysteresis behaviour (no
flicker on borderline values), all without any Qt GUI dependency.
