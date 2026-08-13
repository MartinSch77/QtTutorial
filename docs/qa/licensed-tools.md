# Licensed Tools (Illustrative Only — Not Run In This Repo's CI)

This repository has no licenses for Axivion Suite, Squish for Qt, Squish
Coco, or Qt Test Center, and none of the workflows under `.github/workflows/`
invoke them. Everything below is **illustrative**: it explains what each
tool would check *specifically in this repo's structure* if a license were
available, with a config snippet showing the shape of that integration — not
a working pipeline.

The reference project, **[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp)**,
does hold licenses for all four and runs them for real as part of its
release gate. Treat that repository as the working example of what "done"
looks like for this layer; this document exists so that gap is explicit and
honest rather than silently missing.

---

## Axivion Suite

**What it would check here:** Axivion's architecture-as-code analysis is the
natural fit for enforcing the onboard/offboard split that
`industries/<name>/onboard-*` and `industries/<name>/offboard-*` are meant to
represent. A real Axivion setup would declare an architecture model where
`industries/*/onboard-*` components are forbidden from depending on
`QtNetwork` or `QtSql`, while `industries/*/offboard-*` components (fleet
dashboards, mission control, telehealth, etc.) are allowed to — because
onboard/embedded targets in this repo's industries (instrument clusters,
primary flight displays, patient monitors, satellite telemetry, cab
displays, HMI panels) are modeled as disconnected or resource-constrained,
while their offboard counterparts are explicitly server/dashboard-facing.
Axivion would also run its MISRA C++ / CERT C++ rule sets across
`framework-tour/` and `industries/`, and track cyclomatic-complexity and
clone (duplication) metrics per module as a ratchet, the same role PMD CPD
plays in the open-source layer but with architecture rules layered on top.

**Illustrative config (`.axivion/config.json`-style project pointer):**

```json
{
  "project": "QtTutorial",
  "architecture": {
    "model": "docs/qa/axivion/architecture.gxl",
    "rules": [
      {
        "id": "onboard-no-network-or-sql",
        "from": "industries/*/onboard-*",
        "forbiddenDependencies": ["QtNetwork", "QtSql"],
        "rationale": "Onboard/embedded targets in this repo are modeled as offline or resource-constrained; networking and SQL access belong in the paired offboard-* component."
      }
    ]
  },
  "analyses": ["misra-cpp-2023", "cert-cpp", "clones", "complexity"]
}
```

This is a sketch of the shape of an Axivion project file, not a validated
Axivion schema — see TradingApp's actual `.axivion` setup for the real
thing.

---

## Squish for Qt

**What it would check here:** Squish drives GUI-level smoke tests against
running Widgets and QML/Quick applications — exactly the kind of black-box
"does the app actually come up and respond to clicks" coverage that QTest
unit tests under `tests/framework-tour/` and `tests/industries/` don't
provide, since those exercise non-UI logic (e.g. `UnitConversion`,
`IndicatorGenerator`) in isolation. A real Squish suite here would have one
`.sui` test suite per framework-tour module and per industry example,
scripted to launch the built executable, locate its top-level window or QML
root item by Squish's object map, and assert on visible state (e.g. "the
widgets-basics converter shows the expected converted value after typing
into the input field").

**Illustrative Squish suite skeleton (`tests/gui/widgets-basics/suite.sui`):**

```ini
[SUITE]
VERSION=1
AUT=widgets_basics
OBJECTMAP=script
LANGUAGE=Python
```

paired with a test script such as:

```python
# tests/gui/widgets-basics/tst_convert/test.py  (illustrative — not executed)
import squish

def main():
    squish.startApplication("widgets_basics")
    squish.type(squish.waitForObject(":inputField"), "100")
    squish.test.compare(squish.waitForObjectExists(":resultLabel").text, "100 cm = 1 m")
```

---

## Squish Coco

**What it would check here:** Coco instruments builds to report **MC/DC
(Modified Condition/Decision Coverage)**, which is strictly stronger than
the line/branch coverage this repo's `coverage.yml` workflow already gets
from gcovr. MC/DC matters most on the boolean-condition-heavy logic in this
repo's non-UI libraries (e.g. state-machine guard conditions under
`framework-tour/07-state-machine`, or onboard/offboard threshold checks in
the industry examples) where line coverage can be 100% while individual
condition combinations inside a compound `if` are never independently
exercised. Coco would sit alongside, not instead of, the gcovr reports this
repo already produces.

**Illustrative Coco invocation:**

```bash
# Instrumented build (illustrative — Coco is not installed in this repo's CI)
cov-build --cs "--code-coverage=statement,decision,mcdc" -- \
  cmake --build build --config Debug

# Run the instrumented tests, then generate an MC/DC report
ctest --test-dir build
covwriter csbase.csexe --mcdc -o coverage-report/mcdc.html
```

The `--cs` flag namespace and `csexe`/`csbase` naming above follow Coco's
documented CLI shape; treat this as a sketch of the invocation, not a tested
command line, since Coco is not present in this sandbox.

---

## Qt Test Center

**What it would check here:** Test Center is the headless execution/reporting
layer that would sit on top of *all* of the above (QTest, Squish, Coco) and
aggregate results into a single dashboard — which test targets under
`tests/framework-tour/*` and `tests/industries/*/*` passed/failed, trends
over time, and cross-links back to the requirement IDs in
`requirements/requirements.md`. In this repo's open-source-only setup, that
aggregation role is approximated (much more crudely) by
`tools/trace_report.py`, which produces a plain markdown traceability table
instead of Test Center's dashboard.

**Illustrative Test Center project pointer:**

```yaml
# testcenter-project.yml (illustrative — not executed)
project: QtTutorial
suites:
  - name: framework-tour-unit-tests
    runner: ctest
    resultsDir: build/Testing
  - name: gui-smoke-tests
    runner: squish
    resultsDir: tests/gui/results
reporting:
  linkRequirementsFrom: requirements/requirements.md
```

---

## Summary

| Tool               | Role in this repo (if licensed)                          | Open-source stand-in actually used |
|---------------------|------------------------------------------------------------|-------------------------------------|
| Axivion Suite       | Architecture rules (onboard/offboard split), MISRA/CERT, clone/complexity ratchet | cppcheck, clang-tidy, clazy (`.github/workflows/static-analysis.yml`) — no clone-detection or complexity ratchet tool is wired up in this repo yet |
| Squish for Qt       | GUI-level smoke tests                                       | None — no GUI-level smoke tests exist yet in this repo |
| Squish Coco         | MC/DC coverage                                               | gcovr line/branch/decision coverage only (`docs/qa/coverage`) |
| Qt Test Center      | Headless test execution + reporting dashboard                | `tools/trace_report.py` (plain markdown table) |

For the real, running version of this entire stack, see
https://github.com/MartinSch77/TradingApp.
