# Contributing to QtTutorial

Thanks for considering a contribution. This repository is a teaching
codebase — a Qt 6 / C++23 framework tour plus a set of small
industry-flavored showcase apps — and its QA tooling is deliberately built
entirely from open-source tools (see `docs/qa/licensed-tools.md` for what's
intentionally *not* wired up, and why).

## Before you start

- Read `requirements/requirements.md`. It's short by design and describes
  the handful of structural rules every framework-tour module and industry
  example is expected to follow (has a test directory, doesn't mix onboard
  and offboard concerns, etc).
- If you're adding a new framework-tour module or industry example, look at
  an existing one first (e.g. `framework-tour/01-widgets-basics/` or
  `industries/automotive/`) for the expected directory shape: a
  `CMakeLists.txt` that separates non-UI logic into a static library target
  via `qttutorial_add_app`/`qttutorial_add_test` (see
  `cmake/QtTutorialTargets.cmake`), plus a matching test directory under
  `tests/framework-tour/<name>/` or `tests/industries/<industry>/<name>/`.
- For the broader QA philosophy this repo is modeled on — gating releases on
  static analysis, sanitizers, coverage, and requirements traceability — see
  the reference project, https://github.com/MartinSch77/TradingApp. That
  project also runs paid tools (Axivion, Squish, Coco, Test Center) that
  this repository does not have licenses for; `docs/qa/licensed-tools.md`
  explains what those would check here if they were available.

## Coding conventions

- C++23, Qt 6 (baseline `find_package(Qt6 6.4 ...)` — a specific module can
  document a higher per-module baseline in its own `README.md` if it uses a
  newer Qt feature).
- Keep non-UI logic in a plain library target that a QTest executable can
  link against without pulling in `QApplication`/`QGuiApplication` — this is
  what makes REQ-FW-04 / REQ-IND-03 in `requirements/requirements.md`
  checkable at all.
- New code should compile cleanly under this repo's shared warning flags
  (`cmake/QtTutorialWarnings.cmake`) — don't add module-local suppressions
  unless you've genuinely exhausted better options.
- If your module depends on a Qt add-on module that's only available under
  GPL-3.0-or-later (Qt Graphs today; Qt Charts historically), it needs the
  same per-directory license carve-out documented in
  `docs/qa/licensing.md` — don't assume the repo-wide MIT license covers it.

## Workflow

1. Fork/branch, make your change.
2. Run `tools/run_quality_checks.sh` locally before opening a PR. It
   configures+builds in Debug, runs `ctest`, runs cppcheck and clang-tidy
   (best-effort — it skips gracefully if a tool or `compile_commands.json`
   isn't available), and runs `tools/trace_report.py`. It prints a
   pass/fail summary and exits non-zero if anything failed.
3. Open a PR. CI (`.github/workflows/ci.yml`) builds and tests on Linux,
   Windows and macOS. `.github/workflows/static-analysis.yml` runs cppcheck,
   clang-tidy and clazy — currently informational-only (see that workflow's
   comments for why, and how it's meant to be tightened later), so a
   finding there won't block your PR by itself, but please look at it.
4. If you're adding a new framework-tour module or industry example without
   a matching test directory yet, expect `tools/trace_report.py` to fail —
   that's intentional; add the test directory before merging.

## Commit style

Keep commit messages focused on the "why" of a change, not just a
restatement of the diff. Small, reviewable commits are preferred over one
large one.

## Questions

Open an issue if something in this document (or in
`requirements/requirements.md`) is unclear or seems wrong for a case you've
run into — this is a living teaching repo, and the QA setup is expected to
evolve as more framework-tour modules and industry examples land.
