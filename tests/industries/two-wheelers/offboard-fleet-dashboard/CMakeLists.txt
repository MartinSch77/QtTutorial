qttutorial_add_test(test_two_wheelers_fleet_vehicle_simulator
    SOURCES test_fleet_vehicle_simulator.cpp
    LIB two_wheelers_offboard_fleet_dashboard_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_two_wheelers_telemetry_history_store
    SOURCES test_telemetry_history_store.cpp
    LIB two_wheelers_offboard_fleet_dashboard_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
