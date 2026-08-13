qttutorial_add_test(test_alert_log
    SOURCES test_alert_log.cpp
    LIB central_command_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_asset_history_store
    SOURCES test_asset_history_store.cpp
    LIB central_command_lib
    QT_LIBS Qt6::Core Qt6::Sql
)

qttutorial_add_test(test_fleet_simulator
    SOURCES test_fleet_simulator.cpp
    LIB central_command_lib
    QT_LIBS Qt6::Core
)
