qttutorial_add_test(test_flight_dynamics
    SOURCES test_flight_dynamics.cpp
    LIB onboard_pfd_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_annunciator_logic
    SOURCES test_annunciator_logic.cpp
    LIB onboard_pfd_lib
    QT_LIBS Qt6::Core
)
