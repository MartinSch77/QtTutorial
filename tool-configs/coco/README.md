# Squish Coco — illustrative configuration

**Reference-only**, same caveat as the Axivion/Squish configs alongside
this one: no Coco license is available in this environment, so nothing
here runs in this repo's CI. `coco_build_example.sh` shows the shape of
what running Coco against this project *would* look like: point CMake's
compilers at Coco's `csgcc`/`csg++` instrumenting wrapper compilers, build
and test as normal (the wrappers instrument every translation unit as they
compile), then run `csreport` against the resulting `.csmes` coverage
database.

Why this matters, and why `docs/qa/licensing.md`/`.github/workflows/coverage.yml`
say what they say: this repo's open-source coverage pipeline (`gcovr` over
plain `gcov` data) reports line and branch coverage only. Coco additionally
measures **MC/DC** (Modified Condition/Decision Coverage) — whether every
independent condition in a compound boolean expression has been shown to
independently affect the outcome, not just whether every branch was taken
at least once. That gap is real and this repo does not claim to close it;
the reference project that actually runs Coco and measured the difference
directly (90.5% line coverage hiding 77% MC/DC) is
[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp) — see
its `docs/case-studies/coco.md`.

Treat `coco_build_example.sh`'s exact flags as illustrative starting
points to check against your installed Coco version's own documentation,
not as verified-working commands.
