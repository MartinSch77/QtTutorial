find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

add_library(onboard_wall_panel_lib STATIC
    src/ThermostatModel.h
    src/ThermostatModel.cpp
    src/RoomRegistry.h
    src/RoomRegistry.cpp
    src/LockRegistry.h
    src/LockRegistry.cpp
    src/SecurityCenter.h
    src/SecurityCenter.cpp
    src/SceneRegistry.h
    src/SceneRegistry.cpp
)
target_include_directories(onboard_wall_panel_lib PUBLIC src)
target_link_libraries(onboard_wall_panel_lib PUBLIC Qt6::Core)
target_compile_features(onboard_wall_panel_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(onboard_wall_panel_lib)

qttutorial_add_app(onboard_wall_panel
    SOURCES
        src/main.cpp
        src/RoomListModel.h
        src/RoomListModel.cpp
        src/LockListModel.h
        src/LockListModel.cpp
        src/SecurityListModel.h
        src/SecurityListModel.cpp
        src/ThermostatController.h
        src/ThermostatController.cpp
    QT_LIBS Qt6::Quick Qt6::Qml onboard_wall_panel_lib
    QML_MODULE_URI QtTutorial.HomeAutomation.Onboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/SidebarButton.qml
        src/qml/ToggleSwitch.qml
        src/qml/SliderControl.qml
        src/qml/RoomLightCard.qml
        src/qml/RoomBlindCard.qml
        src/qml/LockTile.qml
        src/qml/SecurityTile.qml
        src/qml/ThermostatGauge.qml
        src/qml/LightingPanel.qml
        src/qml/ClimatePanel.qml
        src/qml/LocksPanel.qml
        src/qml/BlindsPanel.qml
        src/qml/SecurityPanel.qml
        src/qml/HouseIcon.qml
        src/qml/LightbulbIcon.qml
        src/qml/LockIcon.qml
        src/qml/CameraIcon.qml
        src/qml/DoorSensorIcon.qml
        src/qml/ThermostatIcon.qml
        src/qml/SunMoonIcon.qml
        src/qml/SceneCard.qml
        src/qml/ScenesPanel.qml
        src/qml/CameraTile.qml
)
