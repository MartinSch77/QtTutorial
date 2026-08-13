# Tutorial: Custom Rendering and XR (RHI Under QML, Qt Quick 3D Physics/Xr)

This walks through `framework-tour/10-custom-rendering-and-xr/`: two
independent sub-demos, each optional and each guarded to skip cleanly rather
than break this repository's CMake configure step when its Qt module isn't
available on the machine building it — read
[`../../framework-tour/10-custom-rendering-and-xr/README.md`](../../framework-tour/10-custom-rendering-and-xr/README.md)
alongside this for the full build-matrix and licensing details.

## 1. `rhi-under-qml/`: raw `QRhi` drawing composited under QML

### 1.1 The real upstream example, and a correction

This sub-demo adapts Qt's own **"Scene Graph - RHI Under QML"** example
(`qtquick-scenegraph-rhiunderqml-example.html`). Its doc page is explicit
that there are *three* distinct ways to mix custom 2D/3D rendering into a
Qt Quick scene:

1. **Underlay/overlay** via `QQuickWindow::beforeRendering()` /
   `beforeRenderPassRecording()` / `afterRenderPassRecording()` — what the
   real "RHI Under QML" example does, and what `rhi-under-qml/` does here.
2. **Inline rendering** via `QSGRenderNode` — a *different* example,
   "Scene Graph - Custom QSGRenderNode".
3. **Render-to-texture** displayed by an item — yet another example,
   "Scene Graph - RHI Texture Item".

The module this repository was briefed to build described the upstream
example as using `QSGRenderNode`. Reading the actual doc page shows that is
not correct for "RHI Under QML" specifically — it's technique 1, not
technique 2. This module follows the real, doc-verified shape (technique 1)
rather than the brief's description of it.

### 1.2 The QQuickItem/renderer split

`RhiUnderlayItem` (`rhi-under-qml/src/RhiUnderlayItem.h`) is a `QQuickItem`
with `QML_ELEMENT` that never sets `ItemHasContents` and never overrides
`updatePaintNode()` — it draws nothing itself. It exposes one property:

```cpp
Q_PROPERTY(qreal threshold READ threshold WRITE setThreshold NOTIFY thresholdChanged)
```

`qml/Main.qml` animates it:

```qml
RhiUnderlayItem {
    id: underlay
    anchors.fill: parent
    NumberAnimation on threshold {
        from: 0.1; to: 0.9; duration: 2200
        loops: Animation.Infinite
        easing.type: Easing.InOutQuad
    }
}
```

When the item is first attached to a window, it connects — with
`Qt::DirectConnection`, because this signal fires on the Qt Quick render
thread when there is one — to `QQuickWindow::beforeSynchronizing()`. Its
`sync()` slot lazily creates an `RhiUnderlayRenderer` and copies the current
`threshold` value and window pointer into it every frame; this is the
standard scene-graph synchronization pattern (the main thread is blocked
while the render thread runs its sync phase, so this copy is safe without
extra locking).

### 1.3 What the renderer actually does

`RhiUnderlayRenderer::frameStart()` (connected to `beforeRendering()`)
lazily builds the `QRhi` resources on first use — a static vertex buffer for
a 3-vertex triangle, a small dynamic uniform buffer holding `threshold` and
a `yFlip` correction float, one `QRhiShaderResourceBindings`, and one
`QRhiGraphicsPipeline` — then uploads the current `threshold` into the
uniform buffer every frame.

`RhiUnderlayRenderer::mainPassRecordingStart()` (connected to
`beforeRenderPassRecording()`) records the actual draw call:

```cpp
cb->setGraphicsPipeline(m_pipeline.get());
cb->setShaderResources();
cb->setVertexInput(0, 1, &vertexBinding);
cb->draw(3);
```

Because this is recorded onto the *same* command buffer and render pass the
scene graph is about to record its own content into — and specifically
*before* that content, thanks to using `beforeRenderPassRecording()` rather
than `afterRenderPassRecording()` — the triangle ends up underneath the
QML `Rectangle`/`Label` in `qml/Main.qml`. This is a genuine composite of
two different rendering paths in a single frame, not a texture swap or two
overlapping windows.

### 1.4 The fragment shader reads the uniform, live

`shaders/underlay.frag` splits the triangle into two flat colors at
`uv.x == threshold`:

```glsl
layout(std140, binding = 0) uniform buf {
    float threshold;
    float yFlip;
};
void main() {
    float mixAmount = step(threshold, uv.x);
    fragColor = vec4(mix(colorA, colorB, mixAmount), 0.85);
}
```

Because `threshold` is animated from QML and re-uploaded every frame in
`frameStart()`, the color-split boundary visibly sweeps left and right as
the `NumberAnimation` runs — a real, live QML-to-`QRhi` data path, not a
value read once at startup.

### 1.5 Why `<rhi/qrhi.h>` needs its own availability probe

`Qt6::GuiPrivate` is a real CMake target on this repository's local Qt
6.4.2 (backed by the `qt6-base-private-dev` package on this sandbox's
Ubuntu/apt Qt), so a plain `if(NOT TARGET Qt6::GuiPrivate)` guard is not
enough to detect whether this module can actually build locally. The
private RHI headers underneath that target are laid out differently across
Qt versions: this repository's Qt 6.4.2 only has the older
`QtGui/private/qrhi_p.h` path, not the `<rhi/qrhi.h>` path this code (and
Qt's own current example) is written against. `CMakeLists.txt` therefore
does an explicit `check_include_file_cxx("rhi/qrhi.h" ...)` probe against
`Qt6::GuiPrivate`'s include directories, in addition to the usual
`if(NOT TARGET ...)` checks, and skips with `message(STATUS ...)` if the
header genuinely isn't there — this is expected to happen locally in this
repository, and to succeed against CI's Qt 6.11.1.

## 2. `quick3d-physics-xr/`: real rigid-body physics, an XR-ready second entry point

### 2.1 The physics scene

`qml/Main.qml` is an ordinary desktop `View3D`: a `PerspectiveCamera`, a
`DirectionalLight`, one `PhysicsWorld` (`gravity: Qt.vector3d(0, -981, 0)`,
matching the module's own default unit convention), one `StaticRigidBody`
floor using a `PlaneShape`, and several `DynamicRigidBody` boxes using
`BoxShape` that fall under gravity and collide with the floor and each
other — real PhysX-backed simulation via
`import QtQuick3D.Physics` (since Qt 6.4), not a hand-rolled fake.

### 2.2 Avoiding an easy, non-obvious bug: overlapping start positions

Starting several `DynamicRigidBody` shapes stacked at (or very near) the
same position is an easy mistake to make in a "just drop some boxes" demo.
PhysX resolves the resulting interpenetration by shoving the overlapping
bodies apart violently on the very first simulation step, instead of
letting them fall and settle naturally. `src/DropLayout.h`/`.cpp` (pure
C++, `Qt6::Core`-only, no QML/Quick3D/PhysX dependency) exists specifically
to prevent this:

```cpp
[[nodiscard]] static std::vector<DropPosition> gridPositions(
    int count, double radius, double dropHeight);
```

It lays bodies out on a grid, spaced at least `2 * radius` apart in x/z at
a common drop height. `tests/framework-tour/10-custom-rendering-and-xr/test_drop_layout.cpp`
checks the returned count, the drop height, and — across a range of counts
and radii — that no two returned positions are closer than `2 * radius`.

`src/DropLayoutProvider.h`/`.cpp` is a thin `QML_SINGLETON` wrapper (needs
`Qt6::Gui`'s `QVector3D` to build `vector3d`-compatible `QVariant`s, so it
lives outside the pure library) that lets QML call it directly as a
`Repeater3D` model:

```qml
Repeater3D {
    model: DropLayoutProvider.gridPositions(6, window.bodyRadius, 420)
    DynamicRigidBody {
        position: modelData
        collisionShapes: BoxShape { extents: Qt.vector3d(/* ... */) }
        Model { source: "#Cube"; /* ... */ }
    }
}
```

### 2.3 The XR-ready second entry point

`qml/XrMain.qml` is a second, deliberately separate QML file rooted at
`XrView` (`import QtQuick3D.Xr`, since Qt 6.8, **Technology Preview**)
instead of `View3D`/`PerspectiveCamera`, with an `XrOrigin` holding the same
kind of physics scene:

```qml
XrView {
    referenceSpace: XrView.ReferenceSpaceLocalFloor
    xrOrigin: XrOrigin {
        id: origin
        StaticRigidBody { /* ... */ }
        Repeater3D { /* same DropLayoutProvider-driven boxes */ }
    }
    PhysicsWorld { scene: origin; gravity: Qt.vector3d(0, -981, 0) }
}
```

This file is intentionally **not** added to this module's
`qt_add_qml_module(... QML_FILES ...)` list, and is not built, qmllint
-checked, or run anywhere in this repository. Running it for real needs an
XR runtime (e.g. OpenXR) and a headset (or a supported simulator) — neither
this sandbox nor this repository's CI has either, and there is no
meaningful CMake-time way to guard on "a physical headset is attached." It
is kept as documented, XR-ready reference code rather than pretending it
has been exercised.

### 2.4 Licensing: this subdirectory is GPL-3.0-or-later

Qt Quick 3D Physics and Qt Quick 3D Xr are both documented as **"Commercial
or GPLv3"** — the same dual-licensing shape (and the same practical
consequence: no LGPL escape hatch) as Qt Graphs in module 09. Every file
under `quick3d-physics-xr/` therefore carries
`SPDX-License-Identifier: GPL-3.0-or-later` rather than this repository's
usual MIT — see `quick3d-physics-xr/NOTICE.md`. This does not affect
`rhi-under-qml/` (MIT; it only touches LGPL-eligible Qt Gui/Quick/Qml), or
anything else in the repository.

### 2.5 The CMake component name

```cmake
find_package(Qt6 COMPONENTS Quick Qml Quick3D Quick3DPhysics QUIET)
if(NOT TARGET Qt6::Quick3DPhysics)
    message(STATUS "... - skipping quick3d-physics-xr")
    return()
endif()
```

`Quick3DPhysics` is used as the CMake component name, matching the module's
own `Qt6::Quick3DPhysics` CMake target and `QT += quick3dphysics` qmake
feature naming. Qt Quick 3D Physics is not installed anywhere in this
sandbox (it isn't part of the Qt 6.4.2 apt packages this repository's
baseline otherwise uses, on any Qt version available here), so this
specific `find_package` call could not be exercised against a real
installation before relying on CI's Qt 6.11.1 to confirm it — see the
module's `README.md` for the equivalent, and honestly-labeled, caveat about
`Quick3DXr`.

## 3. Try it yourself

- `rhi-under-qml/`: change the two `NumberAnimation` bounds in
  `qml/Main.qml`, or swap `colorA`/`colorB` in `shaders/underlay.frag`, to
  see the effect on the composited underlay.
- `quick3d-physics-xr/`: change the `count`/`radius`/`dropHeight` arguments
  passed to `DropLayoutProvider.gridPositions(...)` in `qml/Main.qml`, or
  add a second `Repeater3D` with `SphereShape`-based bodies, to extend the
  physics scene.
- If you have access to an XR-capable Qt build and headset, try wiring
  `XrMain.qml` into a `qt_add_qml_module` target (or run it standalone via
  `qml XrMain.qml`) to see the same physics scene rendered in real XR.
