find_package(Qt6 REQUIRED COMPONENTS Core Network Quick Qml QuickControls2)

add_library(schafkopf_lib STATIC
    src/Card.h
    src/Card.cpp
    src/TrumpOrder.h
    src/TrumpOrder.cpp
    src/RufspielRules.h
    src/RufspielRules.cpp
    src/SchafkopfProtocol.h
    src/SchafkopfProtocol.cpp
    src/Bot.h
    src/Bot.cpp
    src/SchafkopfGame.h
    src/SchafkopfGame.cpp
    src/SchafkopfClientState.h
    src/SchafkopfClientState.cpp
)
target_include_directories(schafkopf_lib PUBLIC src)
target_link_libraries(schafkopf_lib PUBLIC Qt6::Core games_common_lib)
target_compile_features(schafkopf_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(schafkopf_lib)

qttutorial_add_app(schafkopf
    SOURCES
        src/main.cpp
        src/GameBridge.h
        src/GameBridge.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Network schafkopf_lib
    QML_MODULE_URI QtTutorial.Schafkopf
    QML_SOURCES
        qml/Main.qml
        qml/LobbyScreen.qml
        qml/BiddingScreen.qml
        qml/PlayingScreen.qml
        qml/ResultScreen.qml
        qml/PlayingCard.qml
        qml/CardBack.qml
)
