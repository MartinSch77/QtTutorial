qttutorial_add_test(test_drive_cycle_simulator
    SOURCES test_drive_cycle_simulator.cpp
    LIB onboard_instrument_cluster_lib
    QT_LIBS Qt6::Core
)
