qttutorial_add_test(test_ecg_waveform_generator
    SOURCES test_ecg_waveform_generator.cpp
    LIB onboard_patient_monitor_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_vitals_simulator
    SOURCES test_vitals_simulator.cpp
    LIB onboard_patient_monitor_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_alarm_state_machine
    SOURCES test_alarm_state_machine.cpp
    LIB onboard_patient_monitor_lib
    QT_LIBS Qt6::Core
)
