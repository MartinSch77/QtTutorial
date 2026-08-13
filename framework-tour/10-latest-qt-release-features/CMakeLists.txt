find_package(Qt6 REQUIRED COMPONENTS Quick Qml)
find_package(Qt6 COMPONENTS Graphs QUIET)

if(NOT TARGET Qt6::Graphs)
    message(STATUS "Qt6::Graphs not found (needs Qt >= 6.9) - skipping framework-tour/10-latest-qt-release-features")
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
