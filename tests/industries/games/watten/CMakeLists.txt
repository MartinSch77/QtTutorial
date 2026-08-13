qttutorial_add_test(test_trump_ranking
    SOURCES test_trump_ranking.cpp
    LIB watten_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_bummerl_score
    SOURCES test_bummerl_score.cpp
    LIB watten_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_deal_and_trump
    SOURCES test_deal_and_trump.cpp
    LIB watten_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_watten_game_flow
    SOURCES test_watten_game_flow.cpp
    LIB watten_lib
    QT_LIBS Qt6::Core
)
