qttutorial_add_test(test_hmi_logic
    SOURCES test_hmi_logic.cpp
    LIB onboard_hmi_panel_lib
    QT_LIBS Qt6::Core Qt6::StateMachine
)
