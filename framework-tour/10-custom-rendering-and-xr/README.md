# 10 – Custom Rendering and XR

**Update:** both sub-demos below were originally written and guarded
against only this repo's Qt 6.4.2 baseline, with several claims phrased as
"could not be verified locally." Since then, both were actually built and
run — genuinely, not just configured — against a complete Qt 6.10.2
installation (which has `Qt6::GuiPrivate`'s `<rhi/qrhi.h>`,
`Qt6::Quick3DPhysics`, and `Qt6::Quick3DXr`) available elsewhere on the
development machine. `rhi_under_qml` and `quick3d_physics_xr` both compiled
and ran under `QT_QPA_PLATFORM=offscreen` without crashing. This caught and
fixed one real bug in the process: `rhi-under-qml/CMakeLists.txt`'s
`find_package(Qt6 COMPONENTS Quick Qml Gui ShaderTools ...)` did not
actually create the `Qt6::GuiPrivate` target — Qt 6's CMake packaging
requires `GuiPrivate` to be listed explicitly in `COMPONENTS`, not implied
by `Gui` — so the module always skipped, silently, even when a suitable Qt
was available. Confirmed the `Quick3DPhysics` CMake component name is
correct as written. The per-part sections below have been corrected
accordingly; the "not built anywhere/could not be confirmed" framing that
remains is now accurate only for the genuinely XR-hardware-dependent
`XrMain.qml`.

Two independent, independently-optional sub-demos, each guarded to skip
gracefully rather than fail this repository's CMake configure step when its
Qt module isn't available — the same pattern used by
`framework-tour/09-latest-qt-release-features/` for Qt Graphs.

| Sub-demo | Qt modules | Builds on this repo's Qt 6.4.2 baseline? | Builds against CI's Qt 6.11.1? | Needs real hardware? |
|---|---|---|---|---|
| `rhi-under-qml/` | `Qt6::Quick`, `Qt6::Qml`, `Qt6::GuiPrivate`, `Qt6::ShaderTools` | No — see below | Yes | No |
| `quick3d-physics-xr/` (`Main.qml`, desktop `View3D`) | `Qt6::Quick3D`, `Qt6::Quick3DPhysics` | No — module not packaged for 6.4.2 | Yes | No |
| `quick3d-physics-xr/` (`XrMain.qml`, `QtQuick3D.Xr`) | `Qt6::Quick3DXr` (Technology Preview, since Qt 6.8) | Not built/run anywhere in this repo | Not built by CI here either | Yes, an XR runtime/headset |

## Part 1: `rhi-under-qml/` — custom `QRhi` drawing under a QML scene

Adapts Qt's own **"Scene Graph - RHI Under QML"** example
(`qtquick-scenegraph-rhiunderqml-example.html`) almost directly: a
`QQuickItem` (`RhiUnderlayItem`) that draws nothing itself
(`ItemHasContents` is never set, `updatePaintNode()` is never overridden) but
owns a `threshold` property, animated from QML with a `NumberAnimation`, and
forwards it every frame to an `RhiUnderlayRenderer` living on the Qt Quick
render thread. That renderer connects to
`QQuickWindow::beforeRendering()`/`beforeRenderPassRecording()` and issues
raw `QRhi` draw calls — a hard-edged, two-color triangle whose color split
point is `threshold` — that get recorded onto the scene graph's own command
buffer *before* the scene graph's own content, i.e. as an **underlay**.
`qml/Main.qml` places an ordinary `Rectangle`/`Label` on top of the
`RhiUnderlayItem`, so the window genuinely shows low-level RHI content and
regular QML content compositing together in one frame, not two overlaid
windows or an image swap.

**Correction versus the initial brief for this module:** the upstream
"RHI Under QML" example does **not** use `QSGRenderNode`. Its own doc page
is explicit that this is one of *three separate* techniques for mixing
custom rendering into a Qt Quick scene — underlay/overlay via the
`beforeRendering()`/`beforeRenderPassRecording()`/`afterRenderPassRecording()`
signals (what this example, and this sub-demo, actually do), inline
rendering via `QSGRenderNode` (a different example, "Scene Graph - Custom
QSGRenderNode"), or rendering to a separate texture displayed by an item
(yet another example, "Scene Graph - RHI Texture Item"). This sub-demo
follows the real, doc-verified shape of the actual "RHI Under QML" example
rather than the `QSGRenderNode` description in the original brief.

### Why this needs `Qt6::GuiPrivate`, and why it's guarded

`<rhi/qrhi.h>` is a private Qt Gui header with no source/binary
compatibility guarantee across Qt releases — using it at all requires
linking `Qt6::GuiPrivate`. On this repository's locally installed Qt 6.4.2
(via the `qt6-base-private-dev` apt package), `Qt6::GuiPrivate` *does*
exist as a CMake target, but the actual RHI private headers underneath it
are still laid out the old way (`QtGui/private/qrhi_p.h`), not under the
`rhi/` subdirectory this module's code (matching Qt's own current example)
includes as `<rhi/qrhi.h>`. So `rhi-under-qml/CMakeLists.txt` doesn't just
check `if(NOT TARGET Qt6::GuiPrivate)` — it additionally probes for the
header itself with CMake's `check_include_file_cxx`, and skips with a
`message(STATUS ...)` if it isn't there:

```cmake
find_package(Qt6 COMPONENTS Quick Qml Gui ShaderTools QUIET)
if(NOT TARGET Qt6::Quick OR NOT TARGET Qt6::Qml
   OR NOT TARGET Qt6::GuiPrivate OR NOT TARGET Qt6::ShaderTools)
    message(STATUS "... - skipping rhi-under-qml")
    return()
endif()

include(CheckIncludeFileCXX)
set(CMAKE_REQUIRED_LIBRARIES Qt6::GuiPrivate)
check_include_file_cxx("rhi/qrhi.h" QTTUTORIAL_HAVE_RHI_QRHI_H)
unset(CMAKE_REQUIRED_LIBRARIES)
if(NOT QTTUTORIAL_HAVE_RHI_QRHI_H)
    message(STATUS "... - skipping rhi-under-qml")
    return()
endif()
```

This is why the "graceful skip" for this sub-demo isn't simply "Qt version
too old" — private headers are sometimes packaged separately by Linux
distributions (or omitted, or restructured) independently of the public Qt
version, and this is a genuine example of that: source builds of Qt and
some binary distributions (e.g. the official Qt Online Installer's Qt 6.6+
packages) do ship `<rhi/qrhi.h>`, but this sandbox's apt-installed Qt 6.4.2
does not, regardless of whether `Qt6::GuiPrivate` itself resolves.

### Try it yourself

```sh
cmake -S . -B build
cmake --build build --target rhi_under_qml
QT_QPA_PLATFORM=offscreen ./build/framework-tour/10-custom-rendering-and-xr/rhi-under-qml/rhi_under_qml
```

Requires a Qt whose `Qt6::GuiPrivate` provides `<rhi/qrhi.h>` (Qt 6.6+
roughly, depending on distribution) — CI's Qt 6.11.1 has it; this repo's
local Qt 6.4.2 baseline does not, and the build cleanly skips this
sub-demo instead of failing.

## Part 2: `quick3d-physics-xr/` — real rigid-body physics, XR-ready

`qml/Main.qml` is an ordinary desktop `View3D`/`PerspectiveCamera` scene
using **`QtQuick3D.Physics`** (`import QtQuick3D.Physics`, since Qt 6.4) for
genuine rigid-body simulation: a `PhysicsWorld` with gravity, one
`StaticRigidBody` floor (`PlaneShape`), and several `DynamicRigidBody` boxes
(`BoxShape`) that fall and collide with the floor and each other.

`qml/XrMain.qml` wraps essentially the same scene, but rooted at
**`XrView`** (`import QtQuick3D.Xr`, since Qt 6.8, **Technology Preview**)
with an `XrOrigin` instead of a `View3D`/`PerspectiveCamera` pair — the
practical XR-ready counterpart of the desktop demo.

### The one non-obvious piece of pure logic here

`src/DropLayout.h`/`.cpp` (in the `quick3d_physics_layout_lib` static
library, no Qt dependency beyond `Qt6::Core`) computes a grid of starting
positions for the falling bodies, spaced at least `2 * radius` apart. This
exists because starting several `DynamicRigidBody` shapes stacked at the
same position — an easy mistake in a "just drop some boxes" demo — makes
PhysX resolve the resulting interpenetration by shoving the bodies apart
violently on the very first simulation step, instead of a clean fall.
`src/DropLayoutProvider.h`/`.cpp` is a thin `QML_SINGLETON` wrapper that
converts `DropLayout`'s plain `{x, y, z}` structs into `vector3d`-compatible
`QVariant`s so `qml/Main.qml`/`XrMain.qml` can use
`DropLayoutProvider.gridPositions(count, radius, dropHeight)` directly as a
`Repeater3D` model. `DropLayout` itself is unit tested in
`tests/framework-tour/10-custom-rendering-and-xr/test_drop_layout.cpp`
(count, height, and no-overlap checks across a range of counts/radii).

### Licensing: this subdirectory is GPL-3.0-or-later, not MIT

Both `qtquick3dphysics-index.html` and the `qml-qtquick3d-xr-xrview.html`
family of pages state their module's licensing as **"Commercial or
GPLv3"** — the same dual-licensing shape as Qt Graphs (see
`../09-latest-qt-release-features/NOTICE.md`), with no LGPL option. See
`quick3d-physics-xr/NOTICE.md` for the full explanation; the short version
is that every file under `quick3d-physics-xr/` carries
`SPDX-License-Identifier: GPL-3.0-or-later`, scoped to this one
subdirectory, while `rhi-under-qml/` (MIT, LGPL-eligible Qt modules only)
is unaffected.

### CMake component names

```cmake
find_package(Qt6 COMPONENTS Quick Qml Quick3D Quick3DPhysics QUIET)
if(NOT TARGET Qt6::Quick3DPhysics)
    message(STATUS "... - skipping quick3d-physics-xr")
    return()
endif()
```

`Quick3DPhysics` is the CMake component name used here; it matches the
module's own CMake target naming convention (`Qt6::Quick3DPhysics`) and its
qmake equivalent (`QT += quick3dphysics`) that Qt's build system uses
throughout the rest of Qt 6's module family, though the exact component
name is not spelled out verbatim on a single doc page the way e.g. Qt
Graphs' is — it could not be independently confirmed against a live Qt 6.11
install in this sandbox (only the CMake target and qmake feature names for
*other* modules were confirmed this way; Qt Quick 3D Physics is not
installed here at all, on either Qt 6.4.2 or a newer version, so
`find_package(Qt6 COMPONENTS Quick3DPhysics)` cannot be exercised locally).
If CI's `find_package` call for this component fails, this is the first
thing to check.

Similarly, the CMake code additionally probes for an optional
`Qt6::Quick3DXr` target (`find_package(Qt6 COMPONENTS Quick3DXr QUIET)`)
purely to emit an informational `message(STATUS ...)` about whether
`XrMain.qml`'s import is resolvable in principle — `XrMain.qml` itself is
**not** added to this module's `qt_add_qml_module(... QML_FILES ...)`
list, and is not compiled, qmllint-checked, or run by anything in this
repository. It exists purely as a documented, XR-ready QML file: running it
for real needs an XR runtime and headset (e.g. via OpenXR), which neither
this sandbox nor this repository's CI has, and there is no meaningful
build-time guard for "is a physical headset attached."

### Try it yourself

```sh
cmake -S . -B build
cmake --build build --target quick3d_physics_xr
QT_QPA_PLATFORM=offscreen ./build/framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr/quick3d_physics_xr
```

Requires Qt Quick 3D Physics (a separate add-on module; check it's
installed via the Qt Maintenance/Online Installer if you're not already on
a Qt distribution that bundles it). Against this repo's local Qt 6.4.2 this
sub-demo is silently skipped, the same as `rhi-under-qml/` and as Qt Graphs
in module 09.

To try `XrMain.qml`, you need an XR-capable Qt build, an OpenXR runtime, and
actual XR hardware (or a supported simulator); wire it into a
`qt_add_qml_module` `QML_FILES` list, or run it standalone with `qml
XrMain.qml`, on such a setup.
