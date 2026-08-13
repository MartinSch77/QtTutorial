# Licensing Breakdown

This repository is not uniformly licensed. Most of it is MIT, but one
directory carries a stronger copyleft license because of the Qt module it
depends on. Read this before copying code out of the repo, and before adding
a new framework-tour module or industry example that pulls in a new Qt
add-on module.

## Summary table

| Path                                                | License              | Reason |
|------------------------------------------------------|----------------------|--------|
| Everything except the row below                     | MIT (see `LICENSE`)  | Default project license. |
| `framework-tour/10-latest-qt-release-features/`      | GPL-3.0-or-later     | Uses the Qt Graphs module. |

## Why `framework-tour/10-latest-qt-release-features/` is GPL-3.0-or-later

That module demonstrates Qt Graphs, the successor to the old Qt Charts /
Qt Data Visualization modules. Like Qt Charts before it, **Qt Graphs is only
available under GPL-3.0-or-later or a commercial Qt license — there is no
LGPL option.** Because the module statically/dynamically links against
QtGraphs, any redistributed binary of this module is bound by the GPL, and
so — to avoid license confusion — the source under that directory is
licensed GPL-3.0-or-later rather than the repo-wide MIT license.

This mirrors the documented reasoning in the reference project,
[MartinSch77/TradingApp](https://github.com/MartinSch77/TradingApp), which
carves out the same exception for the module in its tree that uses Qt
Charts: the module's *own* code is offered under the copyleft license that
its mandatory Qt dependency requires, while the rest of that repository
stays under its default permissive license. The same logic applies here,
one module later in the Qt product line (Qt Graphs instead of Qt Charts),
for the same underlying reason (no LGPL tier for that particular
visualization module).

Practically, this means:

- Do not copy code out of `framework-tour/10-latest-qt-release-features/`
  into a project that isn't itself GPL-compatible.
- If a future framework-tour module or industry example adds a dependency
  on another GPL-only Qt add-on (Qt Charts itself, if ever reintroduced, is
  the other historical example), it should get the same per-directory
  license carve-out and an entry in the table above — don't assume the
  repo-wide MIT license silently covers it.
- Everything else in the repo — including code that merely *reads about* or
  links against LGPL-licensed Qt modules (Widgets, Quick, Qml, Network,
  Sql, SerialPort, StateMachine, etc.) — has no such constraint and stays
  MIT.

## Qt itself

Qt is not owned by this repository and is not relicensed by anything above.
Qt is used here under its own dual licensing:

- Most Qt modules (Core, Widgets, Quick/Qml, Network, Sql, SerialPort,
  StateMachine/Scxml, Svg, ShaderTools, Qt5Compat, etc.) are available under
  **LGPLv3** (or a commercial Qt license), which this repository satisfies
  by dynamic linking and by not modifying Qt itself.
- A small number of modules — Qt Charts historically, and **Qt Graphs**
  today — are **GPLv3-or-later only**, with no LGPL tier. Any framework-tour
  module or industry example that opts into one of those modules inherits
  that constraint, as documented above.

See https://www.qt.io/licensing/ for Qt's own authoritative licensing page;
this document only explains how that licensing propagates into *this*
repository's directory structure.
