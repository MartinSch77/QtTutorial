qttutorial_add_test(test_trump_order
    SOURCES test_trump_order.cpp
    LIB schafkopf_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_trick_winner
    SOURCES test_trick_winner.cpp
    LIB schafkopf_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_rufspiel_rules_call
    SOURCES test_rufspiel_rules_call.cpp
    LIB schafkopf_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_rufspiel_rules_play
    SOURCES test_rufspiel_rules_play.cpp
    LIB schafkopf_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_card_scoring
    SOURCES test_card_scoring.cpp
    LIB schafkopf_lib
    QT_LIBS Qt6::Core
)
