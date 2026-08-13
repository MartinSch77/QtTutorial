find_package(Qt6 REQUIRED COMPONENTS Core Network Quick Qml QuickControls2 Quick3D)

add_library(kicker_lib STATIC
    src/KickerTypes.h
    src/KickerSimulation.h
    src/KickerSimulation.cpp
    src/KickerProtocol.h
    src/KickerProtocol.cpp
    src/KickerMatch.h
    src/KickerMatch.cpp
)
target_include_directories(kicker_lib PUBLIC src)
target_link_libraries(kicker_lib PUBLIC Qt6::Core Qt6::Network games_common_lib)
target_compile_features(kicker_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(kicker_lib)

qttutorial_add_app(kicker
    SOURCES
        src/main.cpp
        src/KickerController.h
        src/KickerController.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Quick3D kicker_lib
    QML_MODULE_URI QtTutorial.Kicker
    QML_SOURCES
        qml/Main.qml
        qml/LobbyScreen.qml
        qml/GameScreen.qml
        qml/Table3D.qml
        qml/Hud.qml
)
