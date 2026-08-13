# Licensing notice for this showcase

Every other app in this repository is MIT-licensed. **This one is not** —
its files carry `SPDX-License-Identifier: GPL-3.0-or-later` instead — for
one specific, load-bearing reason: it optionally links against **Qt
Graphs** when it's available (`if(TARGET Qt6::Graphs)` in
`CMakeLists.txt`, for `PriceChartGraphs.qml`'s interactive price chart).

Qt Graphs is available under the same dual-licensing terms Qt itself uses:
**GPLv3-or-commercial**, with **no LGPL option** — unlike the vast
majority of Qt modules this repository otherwise links (Core, Gui,
Widgets, Quick, Network, Sql, ...). Because this app's own `CMakeLists.txt`
conditionally links `Qt6::Graphs` on any Qt install that has it (which
includes CI's Qt 6.11.1), a binary built from this directory in that
configuration is bound by the GPL. A single CMake target cannot ship two
different licenses depending on which Qt it happened to be built against,
so the whole `showcases/stock-tracker/` directory is licensed
GPL-3.0-or-later — the same choice this repository already makes for
`framework-tour/09-latest-qt-release-features/` and
`framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/`, and for
the same underlying reason. See `docs/qa/licensing.md` for the full,
repo-wide breakdown.

This obligation is scoped to *this directory only* — it does not affect
`showcases/news-ticker/` or `showcases/startrek-bridge/`, which have no
GPL-only Qt dependency and remain MIT.

If you fork this repository and do not want any GPL-licensed code in it,
delete `showcases/stock-tracker/` (and its test directory) — nothing else
in the repository depends on it.
