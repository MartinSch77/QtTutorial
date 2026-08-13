find_package(Qt6 REQUIRED COMPONENTS Quick Qml QuickControls2 Network)

add_library(skat_lib STATIC
    src/Card.h
    src/Card.cpp
    src/Deck.h
    src/Deck.cpp
    src/Trump.h
    src/Trump.cpp
    src/Bidding.h
    src/Bidding.cpp
    src/Bot.h
    src/Bot.cpp
    src/SkatGame.h
    src/SkatGame.cpp
)
target_include_directories(skat_lib PUBLIC src)
target_link_libraries(skat_lib PUBLIC Qt6::Core games_common_lib)
target_compile_features(skat_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(skat_lib)

qttutorial_add_app(skat
    SOURCES
        src/main.cpp
        src/SkatController.h
        src/SkatController.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Network skat_lib
    QML_MODULE_URI QtTutorial.Skat
    QML_SOURCES
        qml/Main.qml
        qml/CardView.qml
        qml/HandView.qml
        qml/TrickView.qml
        qml/BiddingPanel.qml
        qml/DiscardPanel.qml
        qml/AnnouncementPanel.qml
        qml/ResultPanel.qml
        qml/JoinPanel.qml
)
