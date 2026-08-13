find_package(Qt6 REQUIRED COMPONENTS Quick Qml)
find_package(Qt6 COMPONENTS Graphs QUIET)

if(NOT TARGET Qt6::Graphs)
    message(STATUS "Qt6::Graphs not found (needs Qt >= 6.9) - skipping framework-tour/09-latest-qt-release-features")
    return()
endif()

# This module deliberately uses LineSeries.strokeStyle/dashPattern/dashOffset/
# joinStyle, which are new *in* Qt 6.11 (not merely "supported as of" it --
# see the README/tutorial doc for why that distinction is the whole point of
# this module). Qt Graphs itself exists from 6.9, so the plain TARGET check
# above is not sufficient: against 6.9/6.10 it configures fine but fails at
# QML load time with "Cannot assign to non-existent property" -- confirmed by
# actually running this module against a real Qt 6.10.2 install. Require
# 6.11+ explicitly instead of only checking that the Graphs target exists.
if(Qt6_VERSION VERSION_LESS "6.11.0")
    message(STATUS "Qt ${Qt6_VERSION} has Qt6::Graphs but not the 6.11+ LineSeries dash/join properties this module uses - skipping framework-tour/09-latest-qt-release-features")
    return()
endif()

add_library(graphs_showcase_lib STATIC
    src/TelemetryGenerator.h
    src/TelemetryGenerator.cpp
)
target_include_directories(graphs_showcase_lib PUBLIC src)
target_link_libraries(graphs_showcase_lib PUBLIC Qt6::Core)
target_compile_features(graphs_showcase_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(graphs_showcase_lib)

qttutorial_add_app(latest_qt_release_features
    SOURCES
        src/main.cpp
        src/TelemetryFeed.h
        src/TelemetryFeed.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::Graphs graphs_showcase_lib
    QML_MODULE_URI QtTutorial.LatestQtFeatures
    QML_SOURCES qml/Main.qml
)
