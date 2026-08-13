find_package(Qt6 REQUIRED COMPONENTS Core Network Quick Qml QuickControls2)

add_library(watten_lib STATIC
    src/Card.h
    src/Card.cpp
    src/Deck.h
    src/Deck.cpp
    src/TrumpRanking.h
    src/TrumpRanking.cpp
    src/BummerlScore.h
    src/BummerlScore.cpp
    src/WattenGame.h
    src/WattenGame.cpp
    src/WattenBot.h
    src/WattenBot.cpp
)
target_include_directories(watten_lib PUBLIC src)
target_link_libraries(watten_lib PUBLIC Qt6::Core)
target_compile_features(watten_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(watten_lib)

qttutorial_add_app(watten
    SOURCES
        src/main.cpp
        src/WattenTableController.h
        src/WattenTableController.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Network watten_lib games_common_lib
    QML_MODULE_URI QtTutorial.Watten
    QML_SOURCES
        qml/Main.qml
        qml/CardFace.qml
        qml/PlayerHand.qml
        qml/TrickArea.qml
        qml/LobbyView.qml
)
