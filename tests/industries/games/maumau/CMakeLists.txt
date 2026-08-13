qttutorial_add_test(test_maumau_deck
    SOURCES test_deck.cpp
    LIB maumau_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_maumau_move_validator
    SOURCES test_move_validator.cpp
    LIB maumau_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_maumau_game
    SOURCES test_maumau_game.cpp
    LIB maumau_lib
    QT_LIBS Qt6::Core games_common_lib
)
