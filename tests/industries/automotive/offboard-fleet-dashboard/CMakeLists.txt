qttutorial_add_test(test_fleet_vehicle_simulator
    SOURCES test_fleet_vehicle_simulator.cpp
    LIB offboard_fleet_dashboard_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_automotive_telemetry_history_store
    SOURCES test_telemetry_history_store.cpp
    LIB offboard_fleet_dashboard_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
