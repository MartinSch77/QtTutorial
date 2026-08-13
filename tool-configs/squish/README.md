# Squish for Qt — illustrative configuration

Another **reference-only** artifact: this repository has no Squish
license, so nothing here runs in CI. `example_suite/` is a real Squish
Test Suite skeleton (`suite.conf` + one test case directory,
`tst_smoke_test/`), shaped as Squish itself expects, pointed at this
repo's `widgets_basics` example (`framework-tour/01-widgets-basics/`) as
a stand-in AUT (Application Under Test).

To adapt this to a real Squish installation against any app in this repo:

1. Open the suite in Squish IDE (or `squishrunner --testsuite tool-configs/squish/example_suite`).
2. Change `AUT` in `suite.conf` to the target binary name (e.g.
   `onboard_patient_monitor`, `schafkopf`, `kicker`).
3. Use Squish's Object Spy against the *running* app to record real object
   names -- `tst_smoke_test/test.py`'s object references
   (`:widgets_basics.Add row_QPushButton`, etc.) are illustrative
   placeholders, not names verified against a real running instance, since
   no Squish install is available in this environment to record them.
4. Duplicate `example_suite/` per app (or per app family — e.g. one suite
   per game, one per industry) as real coverage grows; there's no need for
   a single suite to cover the whole repo.

The reference implementation that actually runs Squish for real GUI
regression tests (7 scenarios, 35 verifications, and a documented bug it
caught on its first real run) is
[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp) — see
its `docs/case-studies/squish.md`.
