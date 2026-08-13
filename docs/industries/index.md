# Industry Showcases

Six industries, each with exactly two example applications:

| Industry | Onboard | Offboard |
|---|---|---|
| [Automotive](automotive.md) | Instrument cluster | Fleet dashboard |
| [Medical](medical.md) | Patient monitor | Telehealth dashboard |
| [Industrial Automation](industrial-automation.md) | HMI panel | Plant SCADA |
| [Railway](railway.md) | Cab display | Fleet ops center |
| [Avionics](avionics.md) | Primary flight display | Fleet maintenance |
| [Space](space.md) | Satellite telemetry | Mission control |

## The onboard / offboard split

Every industry is split the same way, because it's the same split that
shows up in almost every connected product:

- **Onboard** — software that would run embedded *in* the vehicle, machine,
  or device: a fullscreen, kiosk-style Qt Quick (or, where a classic
  hand-painted instrument is the more honest match, `QPainter`-based) UI,
  driven by a real-time simulated sensor/control feed. No network, no
  persistence — every session starts fresh, the way embedded HMIs actually
  boot.
- **Offboard** — software that would run *away* from the machine: a fleet,
  operations, or mission-control dashboard aggregating data as if from many
  onboard units, using `QtNetwork` and/or `QtSql` for persistence and
  history, table/report/trend-oriented rather than instrument-oriented.

Each industry's own doc explains this split concretely for that domain —
what's onboard vs. offboard specifically, which Qt modules each app uses and
why, and what its simulated data models to stay domain-plausible rather than
arbitrary.

## What these are (and are not)

These are technology demonstrations of Qt's reach across industries, built
to be visually and functionally convincing to a non-technical audience while
staying honest about what's simulated. They are **not** certified,
compliance-audited, or production-ready software for any of these domains.
Where a process or domain standard is named in a doc (ISO/IEC/IEEE 12207
generically, ASPICE for automotive specifically, or informational mentions
of DO-178C/ECSS/medical-device expectations), it is context, not a
compliance claim — see each industry doc for specifics.
