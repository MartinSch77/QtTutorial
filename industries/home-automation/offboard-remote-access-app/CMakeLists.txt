find_package(Qt6 REQUIRED COMPONENTS Quick Qml Network Sql)

add_library(offboard_remote_access_app_lib STATIC
    src/Protocol.h
    src/Protocol.cpp
    src/HomeSimulator.h
    src/HomeSimulator.cpp
    src/SnapshotDiff.h
    src/SnapshotDiff.cpp
    src/ActivityLogStore.h
    src/ActivityLogStore.cpp
    src/HomeLinkServer.h
    src/HomeLinkServer.cpp
    src/HomeLinkClient.h
    src/HomeLinkClient.cpp
    src/EnergyMonitor.h
    src/EnergyMonitor.cpp
)
target_include_directories(offboard_remote_access_app_lib PUBLIC src)
target_link_libraries(offboard_remote_access_app_lib PUBLIC Qt6::Core Qt6::Network Qt6::Sql)
target_compile_features(offboard_remote_access_app_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(offboard_remote_access_app_lib)

qttutorial_add_app(offboard_remote_access_app
    SOURCES
        src/main.cpp
        src/RemoteHomeController.h
        src/RemoteHomeController.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::Network Qt6::Sql offboard_remote_access_app_lib
    QML_MODULE_URI QtTutorial.HomeAutomation.Offboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/ToggleSwitch.qml
        src/qml/SliderControl.qml
        src/qml/RemoteRoomRow.qml
        src/qml/RemoteLockRow.qml
        src/qml/RemoteSensorChip.qml
        src/qml/LightbulbIcon.qml
        src/qml/LockIcon.qml
        src/qml/DoorSensorIcon.qml
        src/qml/ThermostatIcon.qml
        src/qml/ActivityIcon.qml
        src/qml/EnergyPanel.qml
)
