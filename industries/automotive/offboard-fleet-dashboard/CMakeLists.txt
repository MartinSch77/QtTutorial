find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(offboard_fleet_dashboard_lib STATIC
    src/FleetVehicleSimulator.h
    src/FleetVehicleSimulator.cpp
    src/FleetModel.h
    src/FleetModel.cpp
    src/TelemetryHistoryStore.h
    src/TelemetryHistoryStore.cpp
)
target_include_directories(offboard_fleet_dashboard_lib PUBLIC src)
target_link_libraries(offboard_fleet_dashboard_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(offboard_fleet_dashboard_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(offboard_fleet_dashboard_lib)

qttutorial_add_app(offboard_fleet_dashboard
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/TrendWidget.h
        src/TrendWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql offboard_fleet_dashboard_lib
)
