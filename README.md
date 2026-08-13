# QtTutorial

*A Qt 6 / C++23 framework tour and multi-industry showcase — for customers,
prospects, managers, and developers evaluating Qt as a cross-platform UI and
application framework.*

[![CI](https://github.com/MartinSch77/QtTutorial/actions/workflows/ci.yml/badge.svg)](https://github.com/MartinSch77/QtTutorial/actions/workflows/ci.yml)
[![Static analysis](https://github.com/MartinSch77/QtTutorial/actions/workflows/static-analysis.yml/badge.svg)](https://github.com/MartinSch77/QtTutorial/actions/workflows/static-analysis.yml)
[![Sanitizers](https://github.com/MartinSch77/QtTutorial/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/MartinSch77/QtTutorial/actions/workflows/sanitizers.yml)
[![Coverage](https://github.com/MartinSch77/QtTutorial/actions/workflows/coverage.yml/badge.svg)](https://github.com/MartinSch77/QtTutorial/actions/workflows/coverage.yml)
[![License: MIT](https://img.shields.io/badge/license-MIT-blue.svg)](LICENSE)

## What this is

This repository is two things at once:

1. **A step-by-step Qt framework tour** (`framework-tour/`) — nine small,
   self-contained, buildable, unit-tested applications that walk through Qt
   Widgets, Qt Quick/QML, Quick Controls styling, networking, concurrency
   (Qt-native and modern C++23), SQL persistence, state machines, serial/
   device I/O, and — as the flagship module — the current Qt release's
   headline features (Qt Graphs, and documentation of Qt GRPC, Quick
   Controls customization, and Qt Safe Renderer).
2. **A multi-industry showcase** (`industries/`) — twelve small applications
   across six industries (Automotive, Medical, Industrial Automation,
   Railway, Avionics, Space), each split into an **onboard** app (what would
   run embedded in the vehicle/machine/device) and an **offboard** app (what
   would run in a fleet/operations/control-room back office), demonstrating
   Qt as a genuinely cross-domain, cross-platform application framework —
   not just a widget toolkit.

Every example — all 21 of them — has real backend logic separated from its
UI, a QTest unit test for that logic, and a README explaining what it
demonstrates and which Qt APIs it exercises. This is deliberately modeled on
the quality-engineering approach of
[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp), scaled
down to what a single open-source tutorial repository can back with
evidence rather than licensed tooling — see [Quality](#quality) below for
exactly what that means and does not mean.

## Quick start

```bash
git clone https://github.com/MartinSch77/QtTutorial.git
cd QtTutorial
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

Requires Qt 6.4 or later (CMake, Ninja/Make, and a C++23 compiler — GCC 13+,
Clang 16+, or MSVC 2022 17.6+). One module
(`framework-tour/09-latest-qt-release-features/`) additionally needs the
**Qt Graphs** module (Qt 6.9+); if it isn't found, CMake skips that one
module and builds everything else — see that module's own README for why.

Run any example directly from `build/`, e.g.:

```bash
./build/framework-tour/01-widgets-basics/widgets_basics
./build/industries/medical/onboard-patient-monitor/onboard_patient_monitor
./build/industries/space/offboard-mission-control/offboard_mission_control
```

Build options (pass as `-D<option>=ON/OFF` to the `cmake -S . -B build` step):

| Option | Default | Effect |
|---|---|---|
| `QTTUTORIAL_BUILD_FRAMEWORK_TOUR` | `ON` | Build `framework-tour/` |
| `QTTUTORIAL_BUILD_INDUSTRIES` | `ON` | Build `industries/` |
| `QTTUTORIAL_BUILD_TESTS` | `ON` | Build and register all QTest suites with `ctest` |
| `QTTUTORIAL_ENABLE_SANITIZERS` | `OFF` | Build with ASan+UBSan |
| `QTTUTORIAL_ENABLE_COVERAGE` | `OFF` | Build with `--coverage` (gcov/lcov/gcovr) |
| `QTTUTORIAL_WARNINGS_AS_ERRORS` | `OFF` | Treat compiler warnings as errors |

## Framework tour

| # | Module | What it demonstrates |
|---|---|---|
| 01 | [`widgets-basics`](framework-tour/01-widgets-basics/) | Qt Widgets, Model/View (`QAbstractTableModel`), `std::expected` |
| 02 | [`qml-quick-basics`](framework-tour/02-qml-quick-basics/) | Qt Quick/QML, `QML_ELEMENT`, list models, states/transitions |
| 03 | [`quick-controls-styling`](framework-tour/03-quick-controls-styling/) | Quick Controls 2, style switching (Basic/Fusion/Material/FluentWinUI3/native) |
| 04 | [`networking`](framework-tour/04-networking/) | `QTcpServer`/`QTcpSocket`, a real client/server protocol |
| 05 | [`concurrency-async`](framework-tour/05-concurrency-async/) | `QtConcurrent` vs. C++23 `std::jthread`/`std::stop_token` |
| 06 | [`sql-persistence`](framework-tour/06-sql-persistence/) | `QtSql`, SQLite, versioned schema migrations, parameterized queries |
| 07 | [`state-machine`](framework-tour/07-state-machine/) | `QStateMachine`, guarded transitions, any-state error handling |
| 08 | [`serial-and-devices`](framework-tour/08-serial-and-devices/) | `QSerialPort`/`QSerialPortInfo`, framing/checksum, simulated transport |
| 09 | [`latest-qt-release-features`](framework-tour/09-latest-qt-release-features/) | Qt Graphs (Qt 6.9+), the current release's headline features |

Each module has its own step-by-step tutorial doc under
[`docs/tutorial/`](docs/tutorial/).

## Industry showcases

See [`docs/industries/index.md`](docs/industries/index.md) for the full
overview, or jump straight to an industry:

| Industry | Onboard | Offboard |
|---|---|---|
| [Automotive](docs/industries/automotive.md) | [Instrument cluster](industries/automotive/onboard-instrument-cluster/) | [Fleet dashboard](industries/automotive/offboard-fleet-dashboard/) |
| [Medical](docs/industries/medical.md) | [Patient monitor](industries/medical/onboard-patient-monitor/) | [Telehealth dashboard](industries/medical/offboard-telehealth-dashboard/) |
| [Industrial Automation](docs/industries/industrial-automation.md) | [HMI panel](industries/industrial-automation/onboard-hmi-panel/) | [Plant SCADA](industries/industrial-automation/offboard-plant-scada/) |
| [Railway](docs/industries/railway.md) | [Cab display](industries/railway/onboard-cab-display/) | [Fleet ops center](industries/railway/offboard-fleet-ops-center/) |
| [Avionics](docs/industries/avionics.md) | [Primary flight display](industries/avionics/onboard-primary-flight-display/) | [Fleet maintenance](industries/avionics/offboard-fleet-maintenance/) |
| [Space](docs/industries/space.md) | [Satellite telemetry](industries/space/onboard-satellite-telemetry/) | [Mission control](industries/space/offboard-mission-control/) |

**Onboard** apps are what would run embedded within the vehicle/machine/
device itself: fullscreen, kiosk-style Qt Quick or hand-painted UIs, driven
by a real-time simulated sensor/control feed, with no network and no
persistence. **Offboard** apps are what would run away from the machine: a
fleet/operations/back-office dashboard aggregating data as if from many
onboard units, using `QtNetwork` and/or `QtSql` for persistence and history.
Every app's simulated data is domain-plausible (e.g. speed correlates with
RPM, a battery only charges outside eclipse, a train's speed respects its
braking curve) rather than arbitrary noise — see each industry's doc for the
specifics.

Process-standard note: these examples do not claim compliance with any
specific standard. Where a process standard is mentioned in the docs, it is
**ISO/IEC/IEEE 12207** (software life-cycle processes) as a generic,
industry-neutral reference — except for the automotive docs, where ASPICE
may also be mentioned, since it is the process framework specific to that
industry. Domain-specific standards (DO-178C for avionics, ECSS for space,
IEC 62304-style expectations for medical) are named only as informational
context, never as a compliance claim.

## Quality

This repository borrows its quality-engineering *approach* from
[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp) — tests
gate the build, static analysis runs on every push, requirements trace to
tests, and a release script refuses to publish on a quality regression — but
implements it entirely with **open-source tooling**, since this project has
no licenses for TradingApp's commercial tools (Squish, Coco, Test Center,
Axivion). See [`docs/qa/licensed-tools.md`](docs/qa/licensed-tools.md) for
what each of those tools would add here if it were available, and
[`docs/qa/licensing.md`](docs/qa/licensing.md) for the one directory that
carries a different (GPL) license than the rest of the repo.

- **Tests**: every framework-tour module and industry example has its
  non-UI logic in a separate library target with a QTest suite, run via
  `ctest` in CI on Linux, Windows, and macOS.
- **Static analysis**: cppcheck, clang-tidy, and clazy run on every push
  ([`.github/workflows/static-analysis.yml`](.github/workflows/static-analysis.yml)),
  informational-first so findings are visible without blocking early
  contributions — see that workflow's comments for how to tighten it.
- **Sanitizers**: ASan+UBSan build and test in CI
  ([`.github/workflows/sanitizers.yml`](.github/workflows/sanitizers.yml)).
- **Coverage**: gcov/lcov/gcovr build and report in CI
  ([`.github/workflows/coverage.yml`](.github/workflows/coverage.yml)).
- **Traceability**: [`tools/trace_report.py`](tools/trace_report.py) checks
  that every module/example in [`requirements/requirements.md`](requirements/requirements.md)
  has a matching test directory, and fails if one is missing — the
  generated table lives at [`docs/qa/traceability.md`](docs/qa/traceability.md).
- **One local script**: [`tools/run_quality_checks.sh`](tools/run_quality_checks.sh)
  runs configure, build, tests, static analysis, and the traceability check
  in one pass — the same gate CI runs, runnable before you push.

## Modern C++

Every module targets **C++23** (`cxx_std_23`), used where it earns its
place rather than for its own sake: `std::expected` for fallible pure
functions (unit conversion, device/protocol parsing), `std::ranges`/views
for pipeline-style data processing, `std::jthread`/`std::stop_token` for
cooperative cancellation contrasted directly against `QtConcurrent`,
`std::format` at the C++/Qt string boundary, and designated initializers
throughout the simulated-data structs.

## Repository layout

```
framework-tour/    9 numbered Qt framework modules (see table above)
industries/        6 industries × {onboard, offboard} = 12 apps
tests/              mirrors framework-tour/ and industries/, one QTest suite per module
docs/
  tutorial/         step-by-step doc per framework-tour module
  industries/       one doc per industry, onboard/offboard explained concretely
  qa/               licensing, licensed-tools-that-aren't-run-here, traceability
requirements/       the lightweight requirements list trace_report.py checks against
tools/              trace_report.py, run_quality_checks.sh
cmake/              shared CMake conventions (warnings, sanitizers, coverage, target helpers)
.github/workflows/  ci.yml, static-analysis.yml, sanitizers.yml, coverage.yml
```

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for the workflow and coding
conventions, and [`SECURITY.md`](SECURITY.md) for how to report an issue —
this is example/teaching software, not a product handling real secrets or
real safety-critical control, and that document says so plainly.

## License

MIT (see [`LICENSE`](LICENSE)) for the whole repository, **except**
[`framework-tour/09-latest-qt-release-features/`](framework-tour/09-latest-qt-release-features/),
which is GPL-3.0-or-later because it links Qt Graphs (no LGPL tier
available for that module) — see
[`docs/qa/licensing.md`](docs/qa/licensing.md) for the full breakdown, and
that module's own [`NOTICE.md`](framework-tour/09-latest-qt-release-features/NOTICE.md).

Qt itself is used under its own LGPLv3/GPLv3 dual licensing; see
https://www.qt.io/licensing/.
