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

This repository is four things at once:

1. **A step-by-step Qt framework tour** (`framework-tour/`) — ten small,
   self-contained, buildable, unit-tested applications that walk through Qt
   Widgets, Qt Quick/QML, Quick Controls styling, networking, concurrency
   (Qt-native and modern C++23), SQL persistence, state machines, serial/
   device I/O, the current Qt release's headline features (Qt Graphs), and
   advanced custom-rendering/XR topics (raw `QRhi` under a QML scene, Qt
   Quick 3D Physics, Qt Quick 3D Xr).
2. **A multi-industry showcase** (`industries/`) — eleven verticals
   (Automotive, Medical, Industrial Automation, Railway, Avionics, Space,
   Two-Wheelers, Agriculture, Mining, Home Automation, Defence), each split
   into an **onboard** app (what would run embedded in the vehicle/machine/
   device) and an **offboard** app (what would run in a fleet/operations/
   control-room back office).
3. **A LAN-multiplayer games category** (`industries/games/`) — five real
   German card games (Schafkopf, Mau-Mau, Watten, Skat) plus a networked,
   mouse-controlled 3D foosball ("Kicker") game, all playable over a local
   network, built on one shared transport layer.
4. **General-purpose visual showcases** (`showcases/`) — a news ticker, a
   "trading cockpit" stock tracker that walks through eight distinct Qt
   capabilities end to end, and an LCARS-styled starship bridge dashboard —
   demonstrating Qt's competitive visual capabilities directly, with no
   industry framing.

Every example has real backend logic separated from its UI, a QTest unit
test for that logic, and a README explaining what it demonstrates and which
Qt APIs it exercises. This is deliberately modeled on the
quality-engineering approach of
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
Clang 16+, or MSVC 2022 17.6+). A handful of modules need newer/optional Qt
add-ons (Qt Graphs 6.9+, with `LineSeries` dash/join styling specifically
needing 6.11+; Qt Quick 3D Physics/Xr; Qt WebSockets 6.5+) — where CMake
can't find one, it skips just that module/feature with a `message(STATUS
...)` and builds everything else; see each module's own README for
specifics. All of this has been verified two ways: against this repo's
declared Qt 6.4 baseline (where the optional pieces gracefully skip) and
against a complete Qt 6.10/6.11 install with every optional module present
(where they all actually build and run) — not just configured.

Run any example directly from `build/`, e.g.:

```bash
./build/framework-tour/01-widgets-basics/widgets_basics
./build/industries/medical/onboard-patient-monitor/onboard_patient_monitor
./build/industries/games/schafkopf/schafkopf
./build/showcases/startrek-bridge/startrek_bridge
```

Build options (pass as `-D<option>=ON/OFF` to the `cmake -S . -B build` step):

| Option | Default | Effect |
|---|---|---|
| `QTTUTORIAL_BUILD_FRAMEWORK_TOUR` | `ON` | Build `framework-tour/` |
| `QTTUTORIAL_BUILD_INDUSTRIES` | `ON` | Build `industries/` (includes `industries/games/`) |
| `QTTUTORIAL_BUILD_GAMES` | `ON` | Build `industries/games/` specifically |
| `QTTUTORIAL_BUILD_SHOWCASES` | `ON` | Build `showcases/` |
| `QTTUTORIAL_BUILD_TESTS` | `ON` | Build and register all QTest suites with `ctest` |
| `QTTUTORIAL_ENABLE_SANITIZERS` | `OFF` | Build with ASan+UBSan |
| `QTTUTORIAL_ENABLE_TSAN` | `OFF` | Build with ThreadSanitizer (mutually exclusive with the above) |
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
| 09 | [`latest-qt-release-features`](framework-tour/09-latest-qt-release-features/) | Qt Graphs, including `LineSeries` properties new *in* Qt 6.11 specifically (GPL-3.0-or-later, see [Licensing](#license)) |
| 10 | [`custom-rendering-and-xr`](framework-tour/10-custom-rendering-and-xr/) | Raw `QRhi` drawing under a QML scene; Qt Quick 3D Physics and Qt Quick 3D Xr (partly GPL-3.0-or-later) |

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
| [Two-Wheelers](docs/industries/two-wheelers.md) | [Rider dashboard](industries/two-wheelers/onboard-rider-dashboard/) | [Fleet dashboard](industries/two-wheelers/offboard-fleet-dashboard/) |
| [Agriculture](docs/industries/agriculture.md) | [Tractor console](industries/agriculture/onboard-tractor-console/) | [Farm operations dashboard](industries/agriculture/offboard-farm-operations-dashboard/) |
| [Mining](docs/industries/mining.md) | [Haul truck console](industries/mining/onboard-haul-truck-console/) | [Pit operations dashboard](industries/mining/offboard-pit-operations-dashboard/) |
| [Home Automation](docs/industries/home-automation.md) | [Wall panel](industries/home-automation/onboard-wall-panel/) | [Remote access app](industries/home-automation/offboard-remote-access-app/) |
| [Defence](docs/industries/defence.md) | [Multi-platform HMI](industries/defence/onboard-multi-platform-hmi/) | [Central command](industries/defence/offboard-central-command/) |

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

**Defence is a passive-visualization showcase, not a weapons system.** It
demonstrates the same kind of situational-awareness/command-and-control
display software real defence primes build with Qt — simulated tracked
objects, sensor/status boards, no weapons guidance, fire-control, or
targeting logic anywhere. See
[`docs/industries/defence.md`](docs/industries/defence.md) for the explicit
framing.

Process-standard note: these examples do not claim compliance with any
specific standard. Where a process standard is mentioned in the docs, it is
**ISO/IEC/IEEE 12207** (software life-cycle processes) as a generic,
industry-neutral reference — except for the automotive docs, where ASPICE
may also be mentioned, since it is the process framework specific to that
industry. Domain-specific standards (DO-178C for avionics, ECSS for space,
IEC 62304-style expectations for medical) are named only as informational
context, never as a compliance claim.

## Games

Five LAN-multiplayer games live under `industries/games/` — structurally
their own vertical (no onboard/offboard split; one app per game, built on
a shared transport rather than each reinventing networking):

| Game | What it is |
|---|---|
| [Schafkopf](industries/games/schafkopf/) | Bavarian trick-taking card game (the "Rufspiel" call-an-ace variant) |
| [Mau-Mau](industries/games/maumau/) | The classic German shedding card game (7/8/Bube rules) |
| [Watten](industries/games/watten/) | Alpine trick-taking card game (one specific, clearly-documented regional variant) |
| [Skat](industries/games/skat/) | The German 3-of-4-players card game, with simplified bidding |
| [Kicker](industries/games/kicker/) | Networked, mouse-controlled 3D foosball — this category's Qt Quick 3D showcase |

[`industries/games/common/`](industries/games/common/) is the shared LAN
transport every game builds on: `TableServer`/`TableClient` (framed JSON
over TCP) for turn-based moves, plus `LanAdvertiser`/`LanBrowser` (UDP
broadcast) so a player can find a game on their network without typing an
IP address. Kicker reuses the same transport for continuous state
broadcast instead of discrete moves — a deliberate contrast documented in
its README. Every game supports 2-4 players, with a basic (explicitly
non-strategic, documented as such) bot filling empty seats so each is
playable and testable solo.

## Showcases

General-purpose, visually striking demos with no industry framing, meant
to demonstrate Qt's capabilities directly:

| Showcase | What it demonstrates |
|---|---|
| [News ticker](showcases/news-ticker/) | Smooth animated scrolling, category filtering, a "breaking news" visual treatment |
| [Stock tracker](showcases/stock-tracker/) | A "trading cockpit" walking through eight Qt capabilities end to end: Quick/QML, Qt Graphs, the Quick scene graph/RHI, WebSockets streaming, a `QStateMachine`-driven order lifecycle, responsive layouts, SVG + Lottie animation, and C++/QML integration (GPL-3.0-or-later, see [Licensing](#license)) |
| [Star Trek bridge](showcases/startrek-bridge/) | An LCARS-styled starship bridge dashboard (original visual language, no copyrighted assets), responsive for desktop and Android-shaped screens |

## Qt framework coverage

[`docs/qa/framework-coverage.md`](docs/qa/framework-coverage.md) is an
honest audit — generated by inspecting every `Qt6::*` link target in the
repo, not asserted from memory — of exactly which Qt Essentials and Add-on
modules this repository demonstrates, and which real, commonly-used parts
of Qt (Multimedia, Positioning/Location, Bluetooth, internationalization,
accessibility, and others) it does **not** cover yet. Read that page
before assuming "the whole framework" means every module.

## Quality

This repository borrows its quality-engineering *approach* from
[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp) — tests
gate the build, static analysis runs on every push, requirements trace to
tests — but implements it with **open-source tooling**, since this project
has no licenses for TradingApp's commercial tools (Squish, Coco, Test
Center, Axivion). See [`docs/qa/licensed-tools.md`](docs/qa/licensed-tools.md)
for what each of those tools would add here if it were available (with
real, checked-in illustrative config files under
[`tool-configs/`](tool-configs/)), and
[`docs/qa/licensing.md`](docs/qa/licensing.md) for the three directories
that carry a different (GPL) license than the rest of the repo.

- **Tests**: every module/example has its non-UI logic in a separate
  library target with a QTest suite, run via `ctest` in CI on Linux,
  Windows, and macOS. 82+ tests, all passing.
- **Static analysis**: cppcheck, clang-tidy, clazy, Clang Static Analyzer,
  `g++ -fanalyzer`, PMD CPD (clone detection), qmllint, and codespell all
  run on every push
  ([`.github/workflows/static-analysis.yml`](.github/workflows/static-analysis.yml)) —
  most informational-first, with a complexity ratchet
  ([`tools/complexity_ratchet.py`](tools/complexity_ratchet.py)) that does
  gate: existing debt is recorded with its number, new or worsened debt
  fails.
- **Sanitizers**: ASan+UBSan and ThreadSanitizer both build and test in CI
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
framework-tour/     10 numbered Qt framework modules (see table above)
industries/         11 industries x {onboard, offboard} = 22 apps
industries/games/    5 LAN-multiplayer games + the shared transport layer
showcases/           3 general-purpose visual demos
tests/               mirrors the above, one QTest suite per module/app
docs/
  tutorial/          step-by-step doc per framework-tour module
  industries/        one doc per industry, onboard/offboard explained concretely
  qa/                licensing, licensed-tools-that-aren't-run-here, traceability,
                     framework coverage audit
requirements/        the lightweight requirements list trace_report.py checks against
tools/               trace_report.py, run_quality_checks.sh, complexity_ratchet.py
tool-configs/        illustrative Axivion/Squish/Coco configs (see docs/qa/licensed-tools.md)
cmake/               shared CMake conventions (warnings, sanitizers, coverage, target helpers)
.github/workflows/   ci.yml, static-analysis.yml, sanitizers.yml, coverage.yml
```

## Contributing

See [`CONTRIBUTING.md`](CONTRIBUTING.md) for the workflow and coding
conventions, and [`SECURITY.md`](SECURITY.md) for how to report an issue —
this is example/teaching software, not a product handling real secrets or
real safety-critical control, and that document says so plainly.

## License

MIT (see [`LICENSE`](LICENSE)) for the whole repository, **except**:

- [`framework-tour/09-latest-qt-release-features/`](framework-tour/09-latest-qt-release-features/) — GPL-3.0-or-later (links Qt Graphs)
- [`framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/`](framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/) — GPL-3.0-or-later (links Qt Quick 3D Physics/Xr)
- [`showcases/stock-tracker/`](showcases/stock-tracker/) — GPL-3.0-or-later (optionally links Qt Graphs when available)

None of these Qt modules have an LGPL tier — see
[`docs/qa/licensing.md`](docs/qa/licensing.md) for the full breakdown and
each directory's own `NOTICE.md`.

Qt itself is used under its own LGPLv3/GPLv3 dual licensing; see
https://www.qt.io/licensing/.
