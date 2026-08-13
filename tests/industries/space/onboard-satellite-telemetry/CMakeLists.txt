qttutorial_add_test(test_orbital_simulator
    SOURCES test_orbital_simulator.cpp
    LIB satellite_telemetry_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_power_system
    SOURCES test_power_system.cpp
    LIB satellite_telemetry_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_thermal_simulator
    SOURCES test_thermal_simulator.cpp
    LIB satellite_telemetry_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_subsystem_health_machine
    SOURCES test_subsystem_health_machine.cpp
    LIB satellite_telemetry_lib
    QT_LIBS Qt6::Core Qt6::StateMachine
)
