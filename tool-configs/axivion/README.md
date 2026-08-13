# Axivion Suite — illustrative configuration

This is a **reference configuration**, not something this repository's CI
runs — there is no Axivion license here. It exists so a reader evaluating
Axivion can see concretely what a project configuration for *this*
repository's layering rules would look like, matching the architecture
checks described in [`docs/qa/licensed-tools.md`](../../docs/qa/licensed-tools.md).

`axivion_config.json` illustrates:

- Pointing Axivion at this project's CMake build (`buildCommand`).
- Defining architecture components along the same lines this repo already
  documents structurally: `framework-tour`, `industries.onboard` /
  `industries.offboard`, `games.common` / `games.title`, and `showcases`.
- Three layering rules that mirror constraints this repo already follows
  by convention (see `requirements/requirements.md` REQ-IND-* and
  REQ-GAME-01) but does not currently enforce with a real static-analysis
  tool: onboard examples must not depend on `QtNetwork`/`QtSql` (that's
  precisely what distinguishes them from their offboard sibling), a game
  may depend on `industries/games/common` but never on a sibling game's
  directory or any other `industries/` vertical, and `showcases/` apps must
  not depend on `industries/` (including `industries/games/`) at all.

The real reference implementation that actually *runs* Axivion (154,000+
real findings, 0 architecture divergences, checked on every push) is
[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp) —
see its `docs/case-studies/axivion.md`. The exact schema Axivion Suite
expects for its own project configuration is proprietary and versioned;
this file is a plausible illustration of *intent*, not a validated,
ready-to-run Axivion project file — treat it as a starting point to adapt
against your installed Axivion Suite version's actual documentation, not
as something to copy verbatim.
