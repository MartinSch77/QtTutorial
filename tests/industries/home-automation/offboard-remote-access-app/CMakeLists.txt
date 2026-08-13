qttutorial_add_test(test_home_simulator
    SOURCES test_home_simulator.cpp
    LIB offboard_remote_access_app_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_snapshot_diff
    SOURCES test_snapshot_diff.cpp
    LIB offboard_remote_access_app_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_activity_log_store
    SOURCES test_activity_log_store.cpp
    LIB offboard_remote_access_app_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
