find_package(Qt6 REQUIRED COMPONENTS Widgets StateMachine)

add_library(satellite_telemetry_lib STATIC
    src/OrbitalSimulator.h
    src/OrbitalSimulator.cpp
    src/PowerSystem.h
    src/PowerSystem.cpp
    src/ThermalSimulator.h
    src/ThermalSimulator.cpp
    src/GroundTrackSimulator.h
    src/GroundTrackSimulator.cpp
    src/SubsystemMonitor.h
    src/SubsystemMonitor.cpp
    src/SubsystemHealthMachine.h
    src/SubsystemHealthMachine.cpp
    src/TelemetrySimulator.h
    src/TelemetrySimulator.cpp
)
target_include_directories(satellite_telemetry_lib PUBLIC src)
target_link_libraries(satellite_telemetry_lib PUBLIC Qt6::Core Qt6::StateMachine)
target_compile_features(satellite_telemetry_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(satellite_telemetry_lib)

qttutorial_add_app(onboard_satellite_telemetry
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/RadialGauge.h
        src/RadialGauge.cpp
        src/SubsystemHealthGrid.h
        src/SubsystemHealthGrid.cpp
        src/IconPainter.h
        src/IconPainter.cpp
        src/StatusIconStrip.h
        src/StatusIconStrip.cpp
    QT_LIBS Qt6::Widgets satellite_telemetry_lib
)
