qttutorial_add_test(test_patient_vitals_simulator
    SOURCES test_patient_vitals_simulator.cpp
    LIB offboard_telehealth_dashboard_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_vitals_history_store
    SOURCES test_vitals_history_store.cpp
    LIB offboard_telehealth_dashboard_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
