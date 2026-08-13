qttutorial_add_test(test_agriculture_field_operation_simulator
    SOURCES test_field_operation_simulator.cpp
    LIB agriculture_offboard_farm_operations_dashboard_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_agriculture_operation_history_store
    SOURCES test_operation_history_store.cpp
    LIB agriculture_offboard_farm_operations_dashboard_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
