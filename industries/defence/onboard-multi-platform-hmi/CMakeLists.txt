find_package(Qt6 REQUIRED COMPONENTS Quick Qml QuickControls2 StateMachine)

add_library(defence_onboard_lib STATIC
    src/TrackMotionModel.h
    src/TrackMotionModel.cpp
    src/VehicleStatusSimulator.h
    src/VehicleStatusSimulator.cpp
    src/SubsystemHealthMonitor.h
    src/SubsystemHealthMonitor.cpp
    src/SubsystemHealthMachine.h
    src/SubsystemHealthMachine.cpp
    src/DetectionSimulator.h
    src/DetectionSimulator.cpp
    src/HudSimulator.h
    src/HudSimulator.cpp
)
target_include_directories(defence_onboard_lib PUBLIC src)
target_link_libraries(defence_onboard_lib PUBLIC Qt6::Core Qt6::StateMachine)
target_compile_features(defence_onboard_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(defence_onboard_lib)

qttutorial_add_app(onboard_multi_platform_hmi
    SOURCES
        src/main.cpp
        src/PlatformSimulator.h
        src/PlatformSimulator.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 defence_onboard_lib
    QML_MODULE_URI QtTutorial.Defence.Onboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/VehicleStatusPanel.qml
        src/qml/HealthGrid.qml
        src/qml/TrackDisplayPanel.qml
        src/qml/DetectionPanel.qml
        src/qml/HudPanel.qml
)
