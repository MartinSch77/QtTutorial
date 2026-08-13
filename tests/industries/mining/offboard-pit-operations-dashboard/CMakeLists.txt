qttutorial_add_test(test_mining_haul_fleet_simulator
    SOURCES test_haul_fleet_simulator.cpp
    LIB mining_offboard_pit_operations_dashboard_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_mining_haul_history_store
    SOURCES test_haul_history_store.cpp
    LIB mining_offboard_pit_operations_dashboard_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
