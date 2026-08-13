# Requirements

This is a lightweight, honest requirements list — not a formal V-model. IDs
follow a simple scheme (`REQ-FW-*` for framework-tour, `REQ-IND-*` for the
industry showcases) so `tools/trace_report.py` can reference them, but no
claim is made that these are exhaustive functional specs for apps this repo
hasn't fully written yet. Each item is meant to be genuinely checkable by
looking at the repo tree or running a script — nothing here describes
detailed behavior of a specific module's UI or business logic that hasn't
been reviewed.

## Framework-tour requirements

- **REQ-FW-01**: Every `framework-tour/<NN-name>/` module has its own
  `CMakeLists.txt` and builds as part of the top-level `framework-tour`
  target when `QTTUTORIAL_BUILD_FRAMEWORK_TOUR` is `ON`.
- **REQ-FW-02**: Every `framework-tour/<NN-name>/` module builds against the
  Qt version declared as this repo's baseline (`find_package(Qt6 6.4 ...)`
  in the root `CMakeLists.txt`) without requiring a newer minimum on its own,
  unless that module's own `README.md` documents a higher per-module
  baseline (e.g. a module demonstrating a Qt feature introduced after 6.4).
- **REQ-FW-03**: Each framework-tour module's non-UI logic (anything not a
  `QWidget`/`QQuickItem`-derived class or `main.cpp`) lives in a separate
  static library target (e.g. `widgets_basics_lib`, `qml_basics_lib`) that
  can be linked into a test executable independent of the GUI.
- **REQ-FW-04**: Each framework-tour module's non-UI logic is covered by at
  least one QTest test executable registered with `ctest` under
  `tests/framework-tour/<name>/`.
- **REQ-FW-05**: No framework-tour module outside
  `framework-tour/09-latest-qt-release-features/` depends on a Qt module
  that is only available under GPL-3.0-or-later (see
  `docs/qa/licensing.md`) — the GPL carve-out is meant to stay isolated to
  that one directory.
- **REQ-FW-06**: Every framework-tour module compiles cleanly under this
  repo's shared warning flags (`qttutorial_set_warnings`, see
  `cmake/QtTutorialWarnings.cmake`) without needing module-specific warning
  suppressions baked into its own `CMakeLists.txt`.

## Industry showcase requirements

- **REQ-IND-01**: Each directory under `industries/` provides exactly one
  `onboard-*` example and exactly one `offboard-*` example, **except**
  `industries/games/`, which is a games category living under
  `industries/` per REQ-GAME-01 and is exempt from the onboard/offboard
  split (see that requirement instead).
- **REQ-IND-02**: Each `industries/<industry>/onboard-*` and
  `industries/<industry>/offboard-*` example has its own `CMakeLists.txt`
  and builds as part of the top-level `industries` target when
  `QTTUTORIAL_BUILD_INDUSTRIES` is `ON`.
- **REQ-IND-03**: Each industry example's core logic is unit-tested
  independent of its UI, i.e. non-UI logic lives in a linkable library
  target that a QTest executable under `tests/industries/<industry>/<name>/`
  links against, mirroring REQ-FW-03/REQ-FW-04 for framework-tour modules.
- **REQ-IND-04**: No `industries/<industry>/onboard-*` example links against
  `Qt6::Network` or `Qt6::Sql`, reflecting a real embedded/offboard
  architectural split: onboard/embedded targets in this repo are modeled as
  disconnected or resource-constrained, while networking and persistence are
  the responsibility of the paired `offboard-*` example.
- **REQ-IND-05**: Each `industries/<industry>/offboard-*` example is
  permitted to (but not required to) link against `Qt6::Network` and/or
  `Qt6::Sql`, since offboard components are explicitly modeled as
  server/dashboard-facing.
- **REQ-IND-06**: Every industry example compiles cleanly under this repo's
  shared warning flags (`qttutorial_set_warnings`) without module-specific
  suppressions.
- **REQ-IND-07**: The set of industries under `industries/` matches the set
  under `tests/industries/` — no industry has example code without a
  corresponding (even if currently empty) test directory tree, and vice
  versa.

## Repo-wide QA process requirements

- **REQ-QA-01**: `ctest` passes (exit code 0) for the full test suite on
  Linux, Windows and macOS in CI (`.github/workflows/ci.yml`).
- **REQ-QA-02**: The test suite also passes when built with
  `QTTUTORIAL_ENABLE_SANITIZERS=ON` (ASan+UBSan), per
  `.github/workflows/sanitizers.yml`.
- **REQ-QA-03**: A gcovr coverage report (HTML + text) is produced for every
  push/PR via `.github/workflows/coverage.yml`, even though no minimum
  coverage percentage is enforced yet — this repo does not claim a coverage
  gate it cannot yet back up with enough tests.
- **REQ-QA-04**: cppcheck, clang-tidy and clazy each run in CI
  (`.github/workflows/static-analysis.yml`) on every push/PR, currently in
  informational (non-blocking) mode, with an explicit documented path to
  making them blocking once the tree is verified clean.
- **REQ-QA-05**: Every framework-tour module and industry example has a
  corresponding entry in the traceability table generated by
  `tools/trace_report.py` (`docs/qa/traceability.md`), and that script exits
  non-zero if any module/example is missing its test directory.
- **REQ-QA-06**: Every per-directory licensing exception in this repo
  (currently four: GPL-3.0-or-later for
  `framework-tour/09-latest-qt-release-features/`,
  `framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/`,
  `showcases/stock-tracker/`, and
  `industries/factory/offboard-digital-twin-control-center/` - see
  `docs/qa/licensing.md`'s summary table for the current, authoritative
  list and why each exists) is documented there before merging, not
  discovered later; the list is expected to grow as new GPL-only Qt add-on
  modules are adopted, so this requirement is about the documentation
  discipline, not about there being exactly one exception.
- **REQ-QA-07**: `docs/qa/licensed-tools.md` clearly labels Axivion Suite,
  Squish for Qt, Squish Coco and Qt Test Center content as illustrative and
  not executed in this repo's CI, with a pointer to
  https://github.com/MartinSch77/TradingApp as the reference implementation
  that runs all four for real.
- **REQ-QA-08**: Clang Static Analyzer (`scan-build`) runs against a
  configure+build of the repo in CI (`.github/workflows/static-analysis.yml`,
  `clang-static-analyzer` job), in informational (non-blocking) mode, with
  its HTML report uploaded as a build artifact.
- **REQ-QA-09**: `g++ -fanalyzer` runs as a separate build in CI
  (`.github/workflows/static-analysis.yml`, `gcc-fanalyzer` job), in
  informational (non-blocking) mode, kept as its own build rather than added
  to the default warning flags in `cmake/QtTutorialWarnings.cmake` so its
  extra compile-time cost doesn't land on every push/PR's main build.
- **REQ-QA-10**: PMD CPD (clone/duplication detection) runs across
  `framework-tour/`, `industries/`, `industries/games/` and `showcases/` in CI
  (`.github/workflows/static-analysis.yml`, `pmd-cpd` job), in informational
  (non-blocking) mode.
- **REQ-QA-11**: `qmllint` runs against every `.qml` file found anywhere
  under `framework-tour/`, `industries/`, `industries/games/` and `showcases/` (found
  generically via `find`, never a hardcoded file list, so it keeps covering
  new QML files as they're added) in CI
  (`.github/workflows/static-analysis.yml`, `qmllint` job) and, when
  `qmllint` is present on PATH, in `tools/run_quality_checks.sh`, in
  informational (non-blocking) mode.
- **REQ-QA-12**: `codespell` runs across `framework-tour/`, `industries/`,
  `industries/games/`, `showcases/`, `docs/`, `tools/`, `requirements/`, `tool-configs/`
  and the repo's top-level markdown files in CI
  (`.github/workflows/static-analysis.yml`, `codespell` job) and in
  `tools/run_quality_checks.sh`, in informational (non-blocking) mode, with
  an explicit, documented `-L` ignore list limited to verified real domain
  terms (not a blanket suppression).
- **REQ-QA-13**: ThreadSanitizer (TSan) runs the full test suite in CI
  (`.github/workflows/sanitizers.yml`, `tsan` job) via a dedicated
  `QTTUTORIAL_ENABLE_TSAN` CMake option (`cmake/QtTutorialSanitizers.cmake`),
  which is mutually exclusive with `QTTUTORIAL_ENABLE_SANITIZERS`
  (ASan+UBSan) and fails the CMake configure step with a clear error if both
  are enabled at once, since ASan and TSan instrumentation cannot be
  combined in the same binary.
- **REQ-QA-14**: `valgrind --tool=memcheck` runs the full test suite (with
  `--trace-children=yes` so ctest's spawned test binaries are actually
  instrumented, not just the `ctest` process itself) in CI
  (`.github/workflows/sanitizers.yml`, `valgrind-memcheck` job) against a
  plain, uninstrumented Debug build, and fails the job
  (`--error-exitcode=1`) on any memcheck finding.
- **REQ-QA-15**: `tools/complexity_ratchet.py` measures per-function
  cyclomatic complexity (via `lizard`) across `framework-tour/`,
  `industries/` (including `industries/games/`) and `showcases/`, fails if any function's
  complexity is not already recorded in `tools/complexity_baseline.json` at
  that value or higher (new or worsened debt), and passes existing debt
  recorded in the baseline through unchanged — this is the one static-check
  job in `.github/workflows/static-analysis.yml`
  (`complexity-ratchet`) that is NOT `continue-on-error`, since it is
  designed to actually gate merges, mirroring TradingApp's documented
  complexity-ratchet philosophy.

## Games requirements

- **REQ-GAME-01**: Every `industries/games/<name>/` game has its own
  `CMakeLists.txt` and builds when `QTTUTORIAL_BUILD_GAMES` is `ON` (via
  `industries/CMakeLists.txt` when industries are built, since games live
  under `industries/` as a vertical without the onboard/offboard split).
  `industries/games/common/` is the shared LAN
  transport (`games_common_lib`: `TableMessage`, `TableServer`,
  `TableClient`, `LanBeacon`, `LanAdvertiser`, `LanBrowser`) every game
  builds on; it is not itself a playable game and is exempt from
  REQ-GAME-04's "playable over LAN" requirement but not from REQ-GAME-02/03.
- **REQ-GAME-02**: Each game's non-UI logic (rules, scoring, deck/card
  representation, physics for the kicker game) lives in a separate static
  library target, independent of both the UI and the network transport.
- **REQ-GAME-03**: Each game's non-UI logic is covered by at least one
  QTest test executable registered with `ctest` under
  `tests/industries/games/<name>/`.
- **REQ-GAME-04**: Each game supports 2-4 players over a LAN, one instance
  hosting via `TableServer` and the others joining via `TableClient`, and
  documents its own message protocol (the game-specific `type`/`payload`
  values it puts in `TableMessage`) in its README.
- **REQ-GAME-05**: Each game's README states which real-world card/table
  game (and, where the source game has significant regional rule variants,
  *which* variant) it implements, so a player can check the digital rules
  against the physical ones they know.

## Showcases requirements

- **REQ-SHOWCASE-01**: Every `showcases/<name>/` app has its own
  `CMakeLists.txt` and builds as part of the top-level `showcases` target
  when `QTTUTORIAL_BUILD_SHOWCASES` is `ON`. Unlike `framework-tour/`
  (teaches one Qt concept) and `industries/` (a vertical, split
  onboard/offboard), `showcases/` apps are general-purpose, visually
  impressive demos with no industry framing and no onboard/offboard split.
- **REQ-SHOWCASE-02**: Each showcase's non-UI logic (data model, feed
  parsing/simulation) is separated into a static library target and covered
  by at least one QTest test executable under `tests/showcases/<name>/`.

## Explicitly out of scope for this list

This list does not attempt to specify the detailed functional behavior of
any individual framework-tour module or industry example (e.g. exact
conversion formulas, exact telemetry fields, exact onboard/offboard message
formats) — those are implementation details of code being written
concurrently by other contributors and are not yet stable enough to pin down
as requirements here. Once a given module or example settles, module-level
requirements can be added as `REQ-FW-<NN>-<seq>` / `REQ-IND-<INDUSTRY>-<seq>`
without renumbering the items above.
