# Licensing Breakdown

This repository is not uniformly licensed. Most of it is MIT, but three
directories carry a stronger copyleft license because of the Qt module
they depend on. Read this before copying code out of the repo, and before
adding a new framework-tour module, industry example, game, or showcase
that pulls in a new Qt add-on module.

## Summary table

| Path                                                          | License              | Reason |
|-----------------------------------------------------------------|----------------------|--------|
| Everything except the rows below                                | MIT (see `LICENSE`)  | Default project license. |
| `framework-tour/09-latest-qt-release-features/`                 | GPL-3.0-or-later     | Uses the Qt Graphs module. |
| `framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/`  | GPL-3.0-or-later     | Uses Qt Quick 3D Physics / Qt Quick 3D Xr. |
| `showcases/stock-tracker/`                                       | GPL-3.0-or-later     | Optionally links Qt Graphs when available (any Qt install with Graphs, including CI's). |

(`industries/games/kicker/` links plain `Qt6::Quick3D`, which — unlike
`Qt6::Quick3DPhysics`/`Qt6::Quick3DXr` — has an LGPL tier, so it is not in
this table and stays MIT.)

## Why these three directories are GPL-3.0-or-later

`framework-tour/09-latest-qt-release-features/` demonstrates Qt Graphs, the
successor to the old Qt Charts / Qt Data Visualization modules. Like Qt
Charts before it, **Qt Graphs is only available under GPL-3.0-or-later or a
commercial Qt license — there is no LGPL option.**

`framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/` uses Qt
Quick 3D Physics and Qt Quick 3D Xr, both documented (per their own doc
pages) as "Commercial or GPLv3" — the same no-LGPL-tier shape.

`showcases/stock-tracker/` is different in *how* the obligation arises,
which is worth spelling out: its `CMakeLists.txt` links `Qt6::Graphs`
*conditionally*, only `if(TARGET Qt6::Graphs)` — on this repository's local
Qt 6.4.2 baseline that branch never triggers, but on CI's Qt 6.11.1 (and
any other Qt 6.9+ install with Graphs present) it does. A single CMake
target, and the resulting binary, cannot carry two different licenses
depending on which Qt happened to build it, so the whole directory is
licensed GPL-3.0-or-later unconditionally — even though a build against an
older Qt produces a binary that never actually links Graphs. See
`showcases/stock-tracker/NOTICE.md` for the app-specific version of this
reasoning.

In all three cases, because the module statically/dynamically links
against a GPL-only Qt module, any redistributed binary is bound by the
GPL, and so — to avoid license confusion — the source under that directory
is licensed GPL-3.0-or-later rather than the repo-wide MIT license.

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

- Do not copy code out of any of the three directories above into a
  project that isn't itself GPL-compatible.
- If a future framework-tour module, industry example, game, or showcase
  adds a dependency on another GPL-only Qt add-on (Qt Charts itself, if
  ever reintroduced, is the other historical example) — including an
  *optional*, guarded dependency like `showcases/stock-tracker/`'s — it
  should get the same per-directory license carve-out and an entry in the
  table above — don't assume the repo-wide MIT license silently covers it,
  and don't assume "it's only linked when available" makes the obligation
  optional too.
- Everything else in the repo — including code that merely *reads about* or
  links against LGPL-licensed Qt modules (Widgets, Quick, Qml, Network,
  Sql, SerialPort, StateMachine, Quick3D, WebSockets, etc.) — has no such
  constraint and stays MIT.

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
