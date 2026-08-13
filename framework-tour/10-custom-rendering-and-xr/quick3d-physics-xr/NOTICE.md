# Licensing notice for this subdirectory

Unlike the rest of the repository (and unlike this module's sibling
`rhi-under-qml/`, which is MIT), every file under `quick3d-physics-xr/`
carries `SPDX-License-Identifier: GPL-3.0-or-later` instead — because this
subdirectory links **Qt Quick 3D Physics** (and, in `XrMain.qml` only,
**Qt Quick 3D Xr**).

Both modules' own documentation index pages state their licensing as
"Commercial or GPLv3" — the same dual-licensing shape as Qt Graphs (see
`../../09-latest-qt-release-features/NOTICE.md`), and with the same
consequence: neither module has an LGPL option, so any binary that links
either of them — statically or dynamically — must either be licensed under
terms compatible with the GPLv3, or be built under a commercial Qt license.
This repository has no commercial Qt license, so `quick3d_physics_xr` (the
executable built from this subdirectory) is GPL-3.0-or-later.

This obligation is scoped to this subdirectory only: it does not affect
`rhi-under-qml/` (which links only Qt6::Quick/Qt6::Qml/Qt6::GuiPrivate, all
available under LGPLv3) or anything else in the repository.

If you fork this repository and do not want any GPL-licensed code in it,
delete `framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/` (and
its test directory, if any) — `rhi-under-qml/` and the rest of the
repository do not depend on it.
