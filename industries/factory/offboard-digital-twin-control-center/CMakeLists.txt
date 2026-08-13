# SPDX-License-Identifier: GPL-3.0-or-later
#
# See NOTICE.md for why this whole directory is GPL-3.0-or-later rather than
# this repository's default MIT license.

find_package(Qt6 COMPONENTS Quick Qml QuickControls2 Quick3D Quick3DParticles QuickTimeline StateMachine QUIET)

# Quick3D is the genuinely load-bearing dependency for this app (the entire
# center pane is a Quick3D View3D scene) - it *is* present in this
# repository's local Qt 6.4.2 baseline, so unlike Quick3DPhysics/Graphs below,
# there is no fallback for it: without it, this whole example is skipped,
# exactly like framework-tour/09-latest-qt-release-features and
# framework-tour/10-custom-rendering-and-xr/quick3d-physics-xr already do for
# their own load-bearing optional modules.
if(NOT TARGET Qt6::Quick3D)
    message(STATUS "Qt6::Quick3D not found - skipping industries/factory/offboard-digital-twin-control-center")
    return()
endif()

# Optional/guarded modules - see README.md and NOTICE.md for the exact
# attempt/fallback decision made for each on this sandbox's local Qt 6.4.2
# baseline (none of these six components are present here; all six are
# real, documentation-verified Qt modules that this app degrades gracefully
# without).
find_package(Qt6 COMPONENTS Quick3DPhysics QUIET)
find_package(Qt6 COMPONENTS Graphs QUIET)
find_package(Qt6 COMPONENTS Multimedia SpatialAudio QUIET)
find_package(Qt6 COMPONENTS Mqtt QUIET)
find_package(Qt6 COMPONENTS TaskTree QUIET)

add_library(factory_control_center_lib STATIC
    src/MachineTelemetrySimulator.h
    src/MachineTelemetrySimulator.cpp
    src/MachineListModel.h
    src/MachineListModel.cpp
    src/MaintenanceWorkflow.h
    src/MaintenanceWorkflow.cpp
    src/DemoConductor.h
    src/DemoConductor.cpp
    src/AlarmAudioController.h
    src/AlarmAudioController.cpp
)
target_include_directories(factory_control_center_lib PUBLIC src)
target_link_libraries(factory_control_center_lib PUBLIC Qt6::Core Qt6::Gui Qt6::StateMachine)
target_compile_features(factory_control_center_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(factory_control_center_lib)

# SurfaceGridBridge includes <QtGraphs/qsurfacedataproxy.h> directly, so it
# can only be compiled when Qt6::Graphs is actually present (see README.md:
# QSurfaceDataProxy::resetArray()/setRow()/setRows() are plain C++ methods,
# not Q_INVOKABLE, so populating a Surface3D chart's data from QML alone -
# what GraphsCharts.qml originally attempted - fails at runtime; this bridge
# is the real fix, not a guarded fallback).
if(TARGET Qt6::Graphs)
    target_sources(factory_control_center_lib PRIVATE
        src/SurfaceGridBridge.h
        src/SurfaceGridBridge.cpp
    )
    target_link_libraries(factory_control_center_lib PUBLIC Qt6::Graphs)
endif()

# Real spatial audio (QAudioEngine/QAudioListener/QSpatialSound - all
# verified real via qt_documentation_read, see AlarmAudioController.h) only
# when both modules are present; otherwise AlarmAudioController falls back
# to its always-available pan/gain math and a visual-only alarm. Neither
# Qt6::Multimedia nor Qt6::SpatialAudio is present in this sandbox.
if(TARGET Qt6::Multimedia AND TARGET Qt6::SpatialAudio)
    target_link_libraries(factory_control_center_lib PUBLIC Qt6::Multimedia Qt6::SpatialAudio)
    target_compile_definitions(factory_control_center_lib PUBLIC OFFBOARD_DIGITAL_TWIN_HAVE_SPATIAL_AUDIO)
    message(STATUS "offboard-digital-twin-control-center: Qt6::Multimedia+Qt6::SpatialAudio found - real QSpatialSound alarm enabled")
else()
    message(STATUS "offboard-digital-twin-control-center: Qt6::Multimedia/Qt6::SpatialAudio not found - alarm is visual-only "
                    "(pan/gain math still computed and unit-tested, see AlarmAudioController)")
endif()

if(TARGET Qt6::TaskTree)
    message(STATUS "offboard-digital-twin-control-center: Qt6::TaskTree found, but is Qt 6.11 Technology Preview - "
                    "MaintenanceWorkflow still uses the hand-rolled QStateMachine implementation documented in its header")
else()
    message(STATUS "offboard-digital-twin-control-center: Qt6::TaskTree not found (Qt 6.11+, Technology Preview) - "
                    "MaintenanceWorkflow uses a hand-rolled QStateMachine instead, see MaintenanceWorkflow.h")
endif()

# Qt6::Mqtt has no embedded-broker component (only QMqttClient) - even where
# available, "no external broker required" would still need a broker
# somewhere. Combined with Qt6::Mqtt not being present in this sandbox
# either, the telemetry source stays the plain in-process C++ signal/slot
# MachineTelemetrySimulator; see its header comment for what a real
# deployment (MQTT/OPC UA/gRPC) would look like instead.
if(TARGET Qt6::Mqtt)
    message(STATUS "offboard-digital-twin-control-center: Qt6::Mqtt found, but QtMqtt ships only a client (no embedded "
                    "broker) - telemetry still comes from the in-process MachineTelemetrySimulator, see its header comment")
else()
    message(STATUS "offboard-digital-twin-control-center: Qt6::Mqtt not found - telemetry comes from the in-process "
                    "MachineTelemetrySimulator (documented MQTT/OPC UA/gRPC stand-in)")
endif()

set(OFFBOARD_DIGITAL_TWIN_QML
    qml/Theme.qml
    qml/Main.qml
    qml/FactoryScene3D.qml
    qml/Machine3D.qml
    qml/EquipmentTree.qml
    qml/MachinePanel.qml
    qml/ChartsPanel.qml
    qml/ChartsFallback.qml
    qml/CommandPalette.qml
    qml/MaintenancePanel.qml
    qml/ThemeSwitcher.qml
    qml/StatusIcon.qml
)

# GraphsCharts.qml does `import QtGraphs`; only bundled into the QML module
# when Qt6::Graphs was actually found, exactly like
# showcases/stock-tracker/qml/PriceChartGraphs.qml. Not found on this
# sandbox's Qt 6.4.2 (needs Qt >= 6.9) - ChartsFallback.qml (plain QtQuick
# Canvas/QPainter) is what actually renders here, chosen at runtime by
# ChartsPanel.qml's Loader via the `graphsAvailable` context property below.
if(TARGET Qt6::Graphs)
    list(APPEND OFFBOARD_DIGITAL_TWIN_QML qml/GraphsCharts.qml)
    message(STATUS "offboard-digital-twin-control-center: Qt6::Graphs found - real Qt Graphs 2D+3D charts enabled")
else()
    message(STATUS "offboard-digital-twin-control-center: Qt6::Graphs not found (needs Qt >= 6.9) - "
                    "using the Canvas-based ChartsFallback.qml only")
endif()

qt6_add_resources(OFFBOARD_DIGITAL_TWIN_ASSETS resources/assets.qrc)

# qt_add_qml_module does NOT auto-detect "pragma Singleton" inside a .qml
# file (confirmed by reading Qt6QmlMacros.cmake: qmldir generation only
# emits the "singleton" keyword when the QT_QML_SINGLETON_TYPE source
# property is set) - without this, Theme was registered as an ordinary
# type, so every Theme.<property> read from other QML files silently
# evaluated to undefined instead of erroring loudly.
set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)

qttutorial_add_app(offboard_digital_twin_control_center
    SOURCES
        src/main.cpp
        ${OFFBOARD_DIGITAL_TWIN_ASSETS}
    QT_LIBS
        Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Quick3D Qt6::Quick3DParticles
        Qt6::QuickTimeline factory_control_center_lib
    QML_MODULE_URI QtTutorial.Factory.OffboardDigitalTwinControlCenter
    QML_SOURCES ${OFFBOARD_DIGITAL_TWIN_QML}
)

if(TARGET Qt6::Graphs)
    target_link_libraries(offboard_digital_twin_control_center PRIVATE Qt6::Graphs)
    target_compile_definitions(offboard_digital_twin_control_center PRIVATE OFFBOARD_DIGITAL_TWIN_HAVE_GRAPHS)
endif()

# Optional, guarded exploded-view via real Qt Quick 3D Physics
# (DynamicRigidBody/CollisionShape, since Qt 6.4, verified via
# qt_documentation_read - see NOTICE.md). Not present in this sandbox;
# Machine3D.qml's procedural NumberAnimation-driven explode (no physics) is
# what actually renders the exploded/transparent inspect view here and on
# any other pre-6.4-Quick3DPhysics or Quick3DPhysics-less Qt install. Present
# purely to document the attempt and keep the licensing story (see
# NOTICE.md) accurate about what this app is built against when available.
if(TARGET Qt6::Quick3DPhysics)
    target_link_libraries(offboard_digital_twin_control_center PRIVATE Qt6::Quick3DPhysics)
    target_compile_definitions(offboard_digital_twin_control_center PRIVATE OFFBOARD_DIGITAL_TWIN_HAVE_QUICK3D_PHYSICS)
    message(STATUS "offboard-digital-twin-control-center: Qt6::Quick3DPhysics found and linked (not yet used for a physics-driven effect on this Qt 6.4.2 sandbox baseline - see NOTICE.md)")
else()
    message(STATUS "offboard-digital-twin-control-center: Qt6::Quick3DPhysics not found - exploded view uses a procedural NumberAnimation instead, see Machine3D.qml")
endif()
