qttutorial_add_test(test_price_simulator
    SOURCES test_price_simulator.cpp
    LIB stock_tracker_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_order_lifecycle
    SOURCES test_order_lifecycle.cpp
    LIB stock_tracker_lib
    QT_LIBS Qt6::Core Qt6::StateMachine
)
