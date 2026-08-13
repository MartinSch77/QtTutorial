qttutorial_add_test(test_track_motion_model
    SOURCES test_track_motion_model.cpp
    LIB defence_onboard_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_vehicle_status_simulator
    SOURCES test_vehicle_status_simulator.cpp
    LIB defence_onboard_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_defence_subsystem_health_machine
    SOURCES test_defence_subsystem_health_machine.cpp
    LIB defence_onboard_lib
    QT_LIBS Qt6::Core Qt6::StateMachine
)

qttutorial_add_test(test_detection_simulator
    SOURCES test_detection_simulator.cpp
    LIB defence_onboard_lib
    QT_LIBS Qt6::Core
)
