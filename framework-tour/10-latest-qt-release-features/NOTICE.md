# Licensing notice for this module

Every other module in this repository is MIT-licensed. **This module is not**
— its files carry `SPDX-License-Identifier: GPL-3.0-or-later` instead — for
one specific, load-bearing reason: it links against **Qt Graphs**.

Qt Graphs (like the Qt Charts module it effectively supersedes) is available
under the same dual-licensing terms Qt itself uses: **GPLv3-or-commercial**.
Unlike the vast majority of Qt modules (Core, Gui, Widgets, Quick, Network,
...), which are also available under the **LGPLv3**, Qt Graphs has **no LGPL
option**. LGPLv3 is what lets a proprietary application dynamically link
against a Qt module without having to release its own source under a
copyleft license (as long as it meets LGPL's linking/relinking obligations).
Because that LGPL path does not exist for Qt Graphs, any binary that links
it — statically or dynamically — must either:

- be licensed under terms compatible with the GPLv3, or
- be built under a commercial Qt license.

This repository has no commercial Qt license, so the only compliant option
for a module that uses Qt Graphs is to distribute it under the GPL. That
obligation is scoped to *this module only* — it does not "infect" the rest
of the repository, precisely because this module lives in its own directory,
links Qt Graphs by itself, and is not linked into any other module's binary.

This mirrors how the reference "TradingApp" project in this repository's
industry showcases handles the older Qt Charts module for exactly the same
reason (GPLv3-or-commercial, no LGPL option): isolate the Charts/Graphs
dependency in its own GPL-licensed target instead of pulling that obligation
into the whole codebase.

If you fork this repository and do not want any GPL-licensed code in it,
delete `framework-tour/10-latest-qt-release-features/` (and its test
directory) — nothing else in the repository depends on it.
