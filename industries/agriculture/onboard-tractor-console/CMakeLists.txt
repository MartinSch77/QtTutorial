find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

add_library(agriculture_onboard_tractor_console_lib STATIC
    src/FieldPassSimulator.h
    src/FieldPassSimulator.cpp
)
target_include_directories(agriculture_onboard_tractor_console_lib PUBLIC src)
target_link_libraries(agriculture_onboard_tractor_console_lib PUBLIC Qt6::Core)
target_compile_features(agriculture_onboard_tractor_console_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(agriculture_onboard_tractor_console_lib)

qttutorial_add_app(agriculture_onboard_tractor_console
    SOURCES
        src/main.cpp
        src/TractorTelemetry.h
        src/TractorTelemetry.cpp
    QT_LIBS Qt6::Quick Qt6::Qml agriculture_onboard_tractor_console_lib
    QML_MODULE_URI QtTutorial.Agriculture.Onboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/Gauge.qml
        src/qml/CrossTrackBar.qml
        src/qml/CoverageArc.qml
)
