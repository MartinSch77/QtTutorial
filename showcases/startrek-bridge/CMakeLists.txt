find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick StateMachine)

add_library(startrek_bridge_lib STATIC
    src/AlertLevel.h
    src/AlertLevel.cpp
    src/NavigationSimulator.h
    src/NavigationSimulator.cpp
    src/EngineeringSimulator.h
    src/EngineeringSimulator.cpp
    src/DeckLifeSupportMonitor.h
    src/DeckLifeSupportMonitor.cpp
    src/DeckLifeSupportMachine.h
    src/DeckLifeSupportMachine.cpp
    src/TacticalSimulator.h
    src/TacticalSimulator.cpp
    src/ShipState.h
    src/ShipState.cpp
)
target_include_directories(startrek_bridge_lib PUBLIC src)
target_link_libraries(startrek_bridge_lib PUBLIC Qt6::Core Qt6::StateMachine)
target_compile_features(startrek_bridge_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(startrek_bridge_lib)

qttutorial_add_app(startrek_bridge
    SOURCES
        src/main.cpp
        src/BridgeSimulator.h
        src/BridgeSimulator.cpp
    QT_LIBS Qt6::Gui Qt6::Qml Qt6::Quick startrek_bridge_lib
    QML_MODULE_URI QtTutorial.Showcases.StartrekBridge
    QML_SOURCES
        src/qml/Main.qml
        src/qml/LcarsElbowFrame.qml
        src/qml/LcarsButton.qml
        src/qml/StationSelector.qml
        src/qml/StatusStrip.qml
        src/qml/Starfield.qml
        src/qml/RadialGauge.qml
        src/qml/DeckGrid.qml
        src/qml/ViewscreenPanel.qml
        src/qml/HelmPanel.qml
        src/qml/EngineeringPanel.qml
        src/qml/TacticalPanel.qml
)
