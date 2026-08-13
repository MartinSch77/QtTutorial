qttutorial_add_test(test_engine_trend
    SOURCES test_engine_trend.cpp
    LIB fleet_maintenance_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_flight_history_store
    SOURCES test_flight_history_store.cpp
    LIB fleet_maintenance_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
