qttutorial_add_test(test_satellite
    SOURCES test_satellite.cpp
    LIB mission_control_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_fleet_orbit_simulator
    SOURCES test_fleet_orbit_simulator.cpp
    LIB mission_control_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_telemetry_history_store
    SOURCES test_telemetry_history_store.cpp
    LIB mission_control_lib
    QT_LIBS Qt6::Core Qt6::Sql
)

qttutorial_add_test(test_ground_track_math
    SOURCES test_ground_track_math.cpp
    LIB mission_control_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_ground_station_tracker
    SOURCES test_ground_station_tracker.cpp
    LIB mission_control_lib
    QT_LIBS Qt6::Core
)
