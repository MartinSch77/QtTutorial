find_package(Qt6 COMPONENTS Quick Qml Quick3D Quick3DPhysics QUIET)

if(NOT TARGET Qt6::Quick3DPhysics)
    message(STATUS "Qt6::Quick3DPhysics not found (separate add-on module, not in this repo's Qt 6.4.2 baseline) - skipping quick3d-physics-xr")
    return()
endif()

add_library(quick3d_physics_layout_lib STATIC
    src/DropLayout.h
    src/DropLayout.cpp
)
target_include_directories(quick3d_physics_layout_lib PUBLIC src)
target_link_libraries(quick3d_physics_layout_lib PUBLIC Qt6::Core)
target_compile_features(quick3d_physics_layout_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(quick3d_physics_layout_lib)

qttutorial_add_app(quick3d_physics_xr
    SOURCES
        src/main.cpp
        src/DropLayoutProvider.h
        src/DropLayoutProvider.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::Quick3D Qt6::Quick3DPhysics quick3d_physics_layout_lib
    QML_MODULE_URI QtTutorial.CustomRenderingXr.Quick3DPhysicsXr
    QML_SOURCES qml/Main.qml
)

# XrMain.qml (import QtQuick3D.Xr) is intentionally not added to QML_SOURCES
# above: QtQuick3D.Xr is Technology Preview and needs an XR runtime/headset
# to run for real, neither of which this repository's CI provides. It is
# left as a plain, documented file - see README.md.
find_package(Qt6 COMPONENTS Quick3DXr QUIET)
if(TARGET Qt6::Quick3DXr)
    message(STATUS "Qt6::Quick3DXr found - XrMain.qml exists but is still not built/tested here (see README.md)")
else()
    message(STATUS "Qt6::Quick3DXr not found - XrMain.qml exists as documentation only (see README.md)")
endif()
