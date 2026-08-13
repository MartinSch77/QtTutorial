find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(mission_control_lib STATIC
    src/Satellite.h
    src/Satellite.cpp
    src/FleetOrbitSimulator.h
    src/FleetOrbitSimulator.cpp
    src/TelemetryHistoryStore.h
    src/TelemetryHistoryStore.cpp
)
target_include_directories(mission_control_lib PUBLIC src)
target_link_libraries(mission_control_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(mission_control_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(mission_control_lib)

qttutorial_add_app(offboard_mission_control
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/SatelliteTableModel.h
        src/SatelliteTableModel.cpp
        src/BatteryTrendWidget.h
        src/BatteryTrendWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql mission_control_lib
)
