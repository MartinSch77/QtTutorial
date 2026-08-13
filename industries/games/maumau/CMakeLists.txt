find_package(Qt6 REQUIRED COMPONENTS Quick Qml QuickControls2 Network)

add_library(maumau_lib STATIC
    src/Card.h
    src/Card.cpp
    src/Deck.h
    src/Deck.cpp
    src/MoveValidator.h
    src/MoveValidator.cpp
    src/CardCodec.h
    src/CardCodec.cpp
    src/MauMauGame.h
    src/MauMauGame.cpp
)
target_include_directories(maumau_lib PUBLIC src)
target_link_libraries(maumau_lib PUBLIC Qt6::Core games_common_lib)
target_compile_features(maumau_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(maumau_lib)

qttutorial_add_app(maumau
    SOURCES
        src/main.cpp
        src/GameController.h
        src/GameController.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Network maumau_lib games_common_lib
    QML_MODULE_URI QtTutorial.MauMau
    QML_SOURCES
        qml/Main.qml
        qml/PlayingCard.qml
        qml/SuitPickerPopup.qml
        qml/LobbyPage.qml
        qml/TablePage.qml
)
