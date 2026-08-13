# Qt Nexus - Digital Twin Control Center

An offboard/supervisory Smart Factory Digital Twin control room: a large
interactive 3D factory scene in the center, an equipment/navigation tree on
the left, a live machine info/controls panel on the right, real-time
charts/alarms at the bottom, a command palette + global search, a theme
switcher, and an automated "Play demo" button that walks through the whole
acceptance-test scenario end-to-end in about 90 seconds.

**License:** this whole directory is `GPL-3.0-or-later`, not this
repository's default MIT - see `NOTICE.md` for exactly why (Qt Quick
Timeline, Qt Graphs, and Qt Quick 3D Physics, all "Commercial or GPLv3").

## What it demonstrates

| Area | Where |
|---|---|
| Qt Quick 3D, procedural PBR scene, `View3D::pick`/`PickResult` selection | `qml/FactoryScene3D.qml`, `qml/Machine3D.qml` |
| Qt Quick 3D Particles (heat shimmer) | `qml/Machine3D.qml`'s `ParticleSystem3D` |
| Qt Quick Timeline (authored camera fly-to keyframes) | `qml/FactoryScene3D.qml`'s `Timeline`/`TimelineAnimation` |
| Qt Graphs 2D+3D (guarded) / Canvas fallback | `qml/GraphsCharts.qml` vs `qml/ChartsFallback.qml`, chosen by `qml/ChartsPanel.qml` |
| Qt Quick 3D Physics (guarded, optional link) | `CMakeLists.txt` |
| Spatial audio (guarded) / pan-gain math fallback | `src/AlarmAudioController.{h,cpp}` |
| Hand-rolled QStateMachine maintenance workflow | `src/MaintenanceWorkflow.{h,cpp}` |
| Hand-rolled QStateMachine demo conductor | `src/DemoConductor.{h,cpp}` |
| In-process telemetry simulator (MQTT stand-in) | `src/MachineTelemetrySimulator.{h,cpp}` |
| Hand-written QML `Theme` singleton (LabsStyleKit stand-in) | `qml/Theme.qml` |
| Qt Lottie | `qml/StatusIcon.qml` / `resources/lottie/system-online.json` |

## The six-step acceptance-test scenario ("Play demo")

`src/DemoConductor` drives all six steps automatically over ~90 seconds; the
same six things can also be done by hand:

1. **Explore the factory** - orbit (drag), zoom (wheel), pan; click a
   machine in the tree or directly in the 3D scene (`View3D::pick`).
2. **A machine overheats** - CNC Mill B2's material animates blue -> orange/
   red over ~6s (`Behavior on baseColor`), heat particles appear, the
   thermal-overlay toggle recolors every machine by temperature, and the
   alarm "moves" with the camera (spatial audio, or its visual-only
   fallback - see below).
3. **Inspect the machine** - selecting it fades its housing to 35% opacity,
   reveals an internal sensor sphere, and highlights the defective part
   with an emissive-color material plus a pulsing opacity animation.
4. **Analyze the problem** - a live vibration waveform, a historical
   temperature line, and a temperature surface (Qt Graphs when available,
   Canvas/QPainter otherwise).
5. **Run the maintenance workflow** - Diagnose -> Stop -> Replace ->
   Restart, with per-step progress, cancel, retry, and an "inject failure"
   demo button to exercise error-recovery deterministically.
6. **Change appearance** - dark/light/high-contrast themes and desktop/
   tablet layout, both instant (plain QML property bindings via
   `qml/Theme.qml`).

## Attempt-first, documented-fallback decisions

Every optional module below was attempted first, `qt_documentation_search`/
`qt_documentation_read`-verified where the API wasn't already certain, and
checked against this sandbox's actual Qt install
(`qmake6 -query QT_INSTALL_PREFIX` -> `/usr`, Qt 6.4.2; confirmed via
`find /usr -iname "Qt6*Config.cmake"` that only the modules already used
elsewhere in this repository - Quick3D, Quick3DParticles, Scxml,
ShaderTools, etc. - are actually installed here):

| Module | Attempted | Available locally? | Result |
|---|---|---|---|
| Qt Graphs (2D+3D) | Yes | No (needs Qt >= 6.9) | `qml/GraphsCharts.qml`, added to the QML module only `if(TARGET Qt6::Graphs)`; `qml/ChartsFallback.qml` (QtQuick `Canvas`) always used here. |
| Qt Quick 3D Physics | Yes | No (this Qt 6.4.2 install has Quick3D but not the separate Quick3DPhysics package) | Linked `if(TARGET Qt6::Quick3DPhysics)`; the exploded/defect-part view actually shipped uses a procedural `NumberAnimation`, not physics - see `qml/Machine3D.qml`. |
| Qt Multimedia + Qt SpatialAudio | Yes (`QSpatialSound`/`QAudioEngine`/`QAudioListener`, verified real via `qt_documentation_read`) | No | `AlarmAudioController` always computes the pan/gain math itself (unit-tested); only drives real `QSpatialSound` playback `#ifdef OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO`. Without it, the alarm is visual-only (a pulsing alarm banner) - the "moves with the camera" behavior is honestly not audible on this build. |
| Qt Mqtt | Yes | No | Even where present, `Qt6::Mqtt` ships only `QMqttClient` (no embedded broker), so "no external broker required" would need one written from scratch either way. Telemetry comes from `MachineTelemetrySimulator`, a plain in-process C++ signal/slot class - see its header comment for what MQTT/OPC UA/gRPC would replace in a real deployment. |
| Qt TaskTree | Yes (`QtTaskTree::Group`/`Do`/`ExecutableItem`, real, `find_package(Qt6 REQUIRED COMPONENTS TaskTree)`) | No - it's Qt 6.11, "Technology Preview" | `MaintenanceWorkflow` uses a hand-rolled `QStateMachine` instead; see its header comment. |
| Qt LabsStyleKit | Yes (`qt_documentation_search` for "LabsStyleKit") | No public QML API found in Qt's documentation at all | `qml/Theme.qml` is a plain hand-written QML singleton instead - no API was guessed. |
| Qt Canvas Painter | Yes (`qt_documentation_search` for "Qt Canvas Painter module") | No matching module found in Qt's documentation | Charts fall back to Qt Graphs (guarded) or plain QtQuick `Canvas`/`Context2D` (always available), not a separate "CanvasPainter" module. |
| Qt Quick Timeline | Yes | **Yes** - `libQt6QuickTimeline.so`/`Qt6QuickTimelineConfig.cmake` are present under this sandbox's active Qt 6.4.2 prefix | Used unconditionally (not guarded) for the camera fly-to keyframes - genuinely built and exercised in this sandbox, not just on CI. |

## What actually builds and runs in this sandbox

`Qt6::Quick3D`, `Qt6::Quick3DParticles`, `Qt6::QuickTimeline`, and
`Qt6::StateMachine` are all present in this sandbox's local Qt 6.4.2
install, so the full GUI app (`offboard_digital_twin_control_center`)
configures, builds, and runs here - not just on CI. Only Qt Graphs, Qt
Quick 3D Physics, Qt Multimedia/SpatialAudio, Qt Mqtt, and Qt TaskTree are
genuinely absent from this baseline (see the table above); those five are
all guarded/optional and degrade to the documented fallback exactly as
described. `CMakeLists.txt`'s only unconditional (non-guarded) prerequisite
is `Qt6::Quick3D` itself - if that one module is ever missing (as it
already is guarded for, mirroring
`framework-tour/09-latest-qt-release-features` and
`framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr`), the whole
directory `return()`s before defining any targets, so the rest of the
repository still configures cleanly.

The C++ static library (`factory_control_center_lib`) and its QTest suite
have no GUI/Quick3D dependency at all and were built/run/verified directly
in this sandbox (see the commit/PR description for the exact `ctest`
results).

## Build & test

```sh
cmake -S . -B build
cmake --build build --target offboard_digital_twin_control_center   # only on a Qt install with Quick Timeline
cmake --build build --target test_machine_telemetry_simulator test_machine_list_model \
    test_maintenance_workflow test_demo_conductor test_alarm_audio_controller
ctest --test-dir build -R "test_(machine_telemetry_simulator|machine_list_model|maintenance_workflow|demo_conductor|alarm_audio_controller)"
```
