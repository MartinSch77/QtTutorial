# Licensing notice for this subdirectory

Unlike the rest of the repository, every file under
`offboard-digital-twin-control-center/` carries
`SPDX-License-Identifier: GPL-3.0-or-later` instead of this repository's
default MIT license, because this app depends on **three** GPL-only Qt
modules - more than any other single directory in this repository (compare
`framework-tour/09-latest-qt-release-features/NOTICE.md`,
`framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/NOTICE.md`, and
`showcases/stock-tracker/NOTICE.md`, each of which cites one or two):

1. **Qt Quick Timeline** (`import QtQuick.Timeline` in
   `qml/FactoryScene3D.qml`, for the authored camera fly-to keyframe
   animation). Its own documentation page
   (`qtquick-timeline-qmlmodule.html`, read via `qt_documentation_read`
   during this app's implementation) states its licensing plainly: **"Qt
   Quick Timeline | Commercial or GPLv3"** - the same no-LGPL-tier shape as
   Qt Graphs. This is a genuinely new finding for this repository: no other
   framework-tour module, industry example, game, or showcase uses Qt Quick
   Timeline yet, so `docs/qa/licensing.md` and `docs/qa/framework-coverage.md`
   are updated accordingly by this change.
2. **Qt Graphs** (`import QtGraphs` in `qml/GraphsCharts.qml`, for the
   vibration/temperature 2D line charts and the `Surface3D` temperature
   surface) - "Commercial or GPLv3", same as
   `framework-tour/09-latest-qt-release-features/` and
   `showcases/stock-tracker/`.
3. **Qt Quick 3D Physics** (`Qt6::Quick3DPhysics`, linked from
   `CMakeLists.txt` when present) - confirmed "Commercial or GPLv3" via
   `qt_documentation_read` on `qml-qtquick3d-physics-dynamicrigidbody.html`
   and `qml-qtquick3d-physics-collisionshape.html`, same as
   `framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/`.

Qt Graphs and Qt Quick 3D Physics are **not** present in this repository's
local Qt 6.4.2 baseline (confirmed by inspecting the active Qt install's
`lib/cmake/Qt6*/` and `lib/*.so` under this sandbox's Qt prefix - no
`Qt6Graphs` or `Qt6Quick3DPhysics` targets/libraries exist there). Both are
therefore **optional, guarded dependencies** in this app, exactly like
`showcases/stock-tracker/`'s `Qt6::Graphs`/`Qt6::WebSockets`:
`CMakeLists.txt` only adds `qml/GraphsCharts.qml` to this app's QML module
and only links `Qt6::Quick3DPhysics` `if(TARGET Qt6::...)`, so a binary
built from this directory on this sandbox's Qt 6.4.2 never actually links
either module. Per the reasoning already established in
`docs/qa/licensing.md` ("an *optional*, guarded dependency ... should get
the same per-directory license carve-out ... and don't assume 'it's only
linked when available' makes the obligation optional too"), that does not
change the outcome: the whole directory is GPL-3.0-or-later regardless of
which Qt install it is built against.

Qt Quick Timeline is different from the other two: it **is** present in
this sandbox (`libQt6QuickTimeline.so`/`Qt6QuickTimelineConfig.cmake` exist
under the active Qt 6.4.2 prefix), and this app's `CMakeLists.txt` links it
unconditionally (no `if(TARGET ...)` guard), so the camera fly-to keyframe
animation in `qml/FactoryScene3D.qml` is genuinely built and exercised
locally, not just on CI. That makes Qt Quick Timeline the one
load-bearing, always-on reason this directory must be GPL-3.0-or-later on
*every* Qt install this app can be built against - Graphs and Quick3D
Physics are additional GPL-only dependencies on top of that, but only ever
conditionally linked.

## What this means practically

- Do not copy code out of `offboard-digital-twin-control-center/` into a
  project that isn't itself GPL-compatible.
- `docs/qa/licensing.md` now lists this directory as a fourth
  GPL-3.0-or-later exception, and spells out that Qt Quick Timeline joins
  Qt Graphs and Qt Quick 3D Physics as a no-LGPL-tier Qt module this
  repository depends on.
- `requirements/requirements.md`'s REQ-QA-06 has been reworded: the
  original wording claimed the `framework-tour/09-...` exception was "the
  only" one, which was already inaccurate by the time
  `quick3d-physics-xr/` and `showcases/stock-tracker/` were added; this is
  now the fourth, and the requirement is worded to say so rather than to
  keep repeating a claim that was already false.

## What is *not* GPL-affected here

Everything else this app links - `Qt6::Quick`, `Qt6::Qml`,
`Qt6::QuickControls2`, `Qt6::Quick3D` (plain, LGPL-tier - see
`docs/qa/licensing.md`'s note on `industries/games/kicker/`),
`Qt6::Quick3DParticles`, `Qt6::StateMachine`, `Qt6::Core`, `Qt6::Gui` - is
available under LGPLv3 (or a commercial Qt license) with no extra
obligation. The attempted-but-not-actually-linked `Qt6::Multimedia`,
`Qt6::SpatialAudio`, `Qt6::Mqtt`, and `Qt6::TaskTree` (see README.md for
why each fell back to a plain substitute on this sandbox) are all
LGPL/permissively-licensed-or-technology-preview modules in their own
right and would not by themselves have forced this directory to be GPL;
Qt Graphs, Qt Quick 3D Physics, and Qt Quick Timeline are what actually do.

If you fork this repository and do not want any GPL-licensed code in it,
delete `industries/factory/offboard-digital-twin-control-center/` (and its
test directory) - nothing else in the repository depends on it.
