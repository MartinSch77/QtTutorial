qttutorial_add_test(test_ship_state
    SOURCES test_ship_state.cpp
    LIB startrek_bridge_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_deck_life_support_machine
    SOURCES test_deck_life_support_machine.cpp
    LIB startrek_bridge_lib
    QT_LIBS Qt6::Core Qt6::StateMachine
)

qttutorial_add_test(test_navigation_simulator
    SOURCES test_navigation_simulator.cpp
    LIB startrek_bridge_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_tactical_simulator
    SOURCES test_tactical_simulator.cpp
    LIB startrek_bridge_lib
    QT_LIBS Qt6::Core
)
