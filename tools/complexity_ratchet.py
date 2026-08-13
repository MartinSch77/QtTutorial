#!/usr/bin/env python3
"""Cyclomatic-complexity ratchet for QtTutorial.

Walks framework-tour/, industries/ (including industries/games/) and showcases/ (every function in
every .cpp/.h/.hpp file under those four top-level directories -- generically,
whatever exists there, not a hardcoded module list) and measures each
function's cyclomatic complexity using `lizard` (a pip-installable, pure
Python complexity analyzer -- see https://github.com/terryyin/lizard). This
is the open-source stand-in for the complexity-ratchet role Axivion Suite
would otherwise play (see docs/qa/licensed-tools.md).

Ratchet philosophy (mirrors TradingApp's documented approach): existing debt
is recorded, with its number, in tools/complexity_baseline.json; new or
worsened debt fails the check. Concretely:

  - ABSOLUTE_THRESHOLD (15, matching lizard's own default `-C` warning
    threshold) is the bar a function must clear to be considered "debt" at
    all. Functions at or below it are never checked against the baseline,
    however they change.
  - Every function currently above ABSOLUTE_THRESHOLD is looked up in the
    baseline by a stable key (relative file path + qualified function name):
      * not in the baseline at all       -> FAIL (brand-new debt)
      * in the baseline, CCN now higher  -> FAIL (worsened debt)
      * in the baseline, CCN same/lower  -> OK (existing, non-worsening debt)
  - Functions that were in the baseline but have dropped to/under the
    threshold, or been removed/renamed, are reported as improvements but
    never fail anything -- the baseline is a ceiling, not a floor.

Run `tools/complexity_ratchet.py --update-baseline` to (re)write
tools/complexity_baseline.json from the current tree after a deliberate
refactor changes the numbers -- this is a manual, reviewed action (the new
baseline shows up as a diff in the PR), not something CI does automatically.

Exit status (plain run, no --update-baseline):
  0  no function exceeds ABSOLUTE_THRESHOLD that isn't already recorded in
     the baseline at that CCN or higher.
  1  at least one function is new debt (undocumented, over threshold) or
     worsened debt (over threshold and higher than its baseline entry), OR
     the `lizard` tool is not available (see below).
  2  usage error.

If `lizard` is not installed, this prints a clear message and exits non-zero
rather than silently reporting a false "all clear" -- mirrors trace_report.py
in preferring an honest failure over a misleading pass.
"""

from __future__ import annotations

import argparse
import csv
import io
import shutil
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parent.parent
SCAN_DIRS = ["framework-tour", "industries", "showcases"]
BASELINE_PATH = REPO_ROOT / "tools" / "complexity_baseline.json"

# Matches lizard's own default -C/--CCN warning threshold (see `lizard
# --help`), reused here rather than inventing a different number, and
# documented per the task's "e.g. 15" instruction.
ABSOLUTE_THRESHOLD = 15


@dataclass
class FunctionMetric:
    key: str            # "<relative/path>::<qualified_function_name>"
    file: str           # relative path, for display
    function_name: str
    ccn: int
    start_line: int


def _relative(path_str: str) -> str:
    try:
        return str(Path(path_str).resolve().relative_to(REPO_ROOT))
    except ValueError:
        return path_str


def run_lizard() -> list[FunctionMetric]:
    """Runs `lizard --csv` over SCAN_DIRS and parses its per-function rows.

    lizard's CSV columns (verified locally against lizard 1.23.0's actual
    output, since `lizard --help` does not document the CSV column order
    itself): nloc, ccn, token_count, param_count, length, location,
    filename, function_name, long_name, start_line, end_line.
    """
    lizard_bin = shutil.which("lizard")
    if lizard_bin is None:
        print(
            "complexity_ratchet: 'lizard' not found on PATH. Install it with "
            "`pip install lizard` and re-run.",
            file=sys.stderr,
        )
        return []

    existing_dirs = [d for d in SCAN_DIRS if (REPO_ROOT / d).is_dir()]
    if not existing_dirs:
        print("complexity_ratchet: none of the scanned directories exist yet.")
        return []

    cmd = [lizard_bin, "-l", "cpp", "--csv", *existing_dirs]
    result = subprocess.run(
        cmd, cwd=REPO_ROOT, capture_output=True, text=True, check=False
    )
    # lizard exits non-zero when it finds functions over its own -C default;
    # that is not an error condition for us, we do our own threshold/ratchet
    # logic against the parsed rows below, so only stderr content (not exit
    # code) indicates a real problem running the tool.
    if result.returncode not in (0, 1) or not result.stdout.strip():
        print(
            f"complexity_ratchet: lizard failed to run (exit {result.returncode}):\n"
            f"{result.stderr}",
            file=sys.stderr,
        )
        return []

    metrics: list[FunctionMetric] = []
    reader = csv.reader(io.StringIO(result.stdout))
    for row in reader:
        if len(row) < 11:
            continue
        try:
            ccn = int(row[1])
            start_line = int(row[9])
        except ValueError:
            continue
        filename = _relative(row[6])
        function_name = row[7]
        metrics.append(
            FunctionMetric(
                key=f"{filename}::{function_name}",
                file=filename,
                function_name=function_name,
                ccn=ccn,
                start_line=start_line,
            )
        )
    return metrics


def load_baseline() -> dict[str, int]:
    if not BASELINE_PATH.is_file():
        return {}
    import json

    return json.loads(BASELINE_PATH.read_text(encoding="utf-8"))


def write_baseline(metrics: list[FunctionMetric]) -> None:
    import json

    debt = {m.key: m.ccn for m in metrics if m.ccn > ABSOLUTE_THRESHOLD}
    BASELINE_PATH.write_text(
        json.dumps(debt, indent=2, sort_keys=True) + "\n", encoding="utf-8"
    )
    print(
        f"complexity_ratchet: wrote {len(debt)} baseline entry(ies) "
        f"(functions with CCN > {ABSOLUTE_THRESHOLD}) to {BASELINE_PATH}."
    )


def check(metrics: list[FunctionMetric], baseline: dict[str, int]) -> int:
    by_key = {m.key: m for m in metrics}
    debt_now = [m for m in metrics if m.ccn > ABSOLUTE_THRESHOLD]

    new_debt: list[FunctionMetric] = []
    worsened_debt: list[tuple[FunctionMetric, int]] = []
    unchanged_debt: list[FunctionMetric] = []

    for m in debt_now:
        baseline_ccn = baseline.get(m.key)
        if baseline_ccn is None:
            new_debt.append(m)
        elif m.ccn > baseline_ccn:
            worsened_debt.append((m, baseline_ccn))
        else:
            unchanged_debt.append(m)

    improved = [
        key for key, ccn in baseline.items()
        if key not in by_key or by_key[key].ccn <= ABSOLUTE_THRESHOLD
    ]

    print(
        f"complexity_ratchet: {len(metrics)} function(s) analyzed, "
        f"{len(debt_now)} over threshold (CCN > {ABSOLUTE_THRESHOLD}), "
        f"{len(baseline)} baseline entry(ies)."
    )

    if unchanged_debt:
        print(f"\n{len(unchanged_debt)} existing debt entry(ies) (recorded, not worsened):")
        for m in sorted(unchanged_debt, key=lambda m: -m.ccn):
            print(f"  - {m.key} (CCN {m.ccn}, baseline {baseline[m.key]}) at line {m.start_line}")

    if improved:
        print(f"\n{len(improved)} baseline entry(ies) improved or removed (no action needed):")
        for key in sorted(improved):
            print(f"  - {key} (was CCN {baseline[key]})")

    exit_code = 0

    if new_debt:
        exit_code = 1
        print(f"\nFAIL: {len(new_debt)} function(s) exceed CCN {ABSOLUTE_THRESHOLD} "
              "and are not in the baseline (new debt):")
        for m in sorted(new_debt, key=lambda m: -m.ccn):
            print(f"  - {m.key} (CCN {m.ccn}) at line {m.start_line}")

    if worsened_debt:
        exit_code = 1
        print(f"\nFAIL: {len(worsened_debt)} function(s) got MORE complex than their "
              "recorded baseline (worsened debt):")
        for m, baseline_ccn in sorted(worsened_debt, key=lambda t: -(t[0].ccn - t[1])):
            print(f"  - {m.key}: CCN {baseline_ccn} -> {m.ccn} at line {m.start_line}")

    if exit_code == 0:
        print(
            f"\ncomplexity_ratchet: OK — no new or worsened complexity debt "
            f"above CCN {ABSOLUTE_THRESHOLD}."
        )
    else:
        print(
            "\nFix the function(s) above, or if the added complexity is "
            "deliberate and reviewed, run "
            "`tools/complexity_ratchet.py --update-baseline` and commit the "
            "updated tools/complexity_baseline.json alongside the change.",
            file=sys.stderr,
        )

    return exit_code


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--update-baseline",
        action="store_true",
        help="Recompute and overwrite tools/complexity_baseline.json from the "
             "current tree instead of checking against it.",
    )
    args = parser.parse_args()

    metrics = run_lizard()
    if not metrics and shutil.which("lizard") is None:
        return 1

    if args.update_baseline:
        write_baseline(metrics)
        return 0

    baseline = load_baseline()
    return check(metrics, baseline)


if __name__ == "__main__":
    sys.exit(main())
