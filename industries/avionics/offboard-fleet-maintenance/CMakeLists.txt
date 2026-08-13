find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(fleet_maintenance_lib STATIC
    src/EngineTrend.h
    src/EngineTrend.cpp
    src/Fleet.h
    src/FleetSimulator.h
    src/FleetSimulator.cpp
    src/FlightHistoryStore.h
    src/FlightHistoryStore.cpp
)
target_include_directories(fleet_maintenance_lib PUBLIC src)
target_link_libraries(fleet_maintenance_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(fleet_maintenance_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(fleet_maintenance_lib)

qttutorial_add_app(offboard_fleet_maintenance
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/FleetTableModel.h
        src/FleetTableModel.cpp
        src/TrendWidget.h
        src/TrendWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql fleet_maintenance_lib
)
