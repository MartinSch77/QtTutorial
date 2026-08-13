qttutorial_add_test(test_skat_trump_order
    SOURCES test_skat_trump_order.cpp
    LIB skat_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_bidding
    SOURCES test_bidding.cpp
    LIB skat_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_skat_pickup_discard
    SOURCES test_skat_pickup_discard.cpp
    LIB skat_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_card_points
    SOURCES test_card_points.cpp
    LIB skat_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_declarer_result
    SOURCES test_declarer_result.cpp
    LIB skat_lib
    QT_LIBS Qt6::Core
)
