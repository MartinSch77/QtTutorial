find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

add_library(onboard_instrument_cluster_lib STATIC
    src/DriveCycleSimulator.h
    src/DriveCycleSimulator.cpp
)
target_include_directories(onboard_instrument_cluster_lib PUBLIC src)
target_link_libraries(onboard_instrument_cluster_lib PUBLIC Qt6::Core)
target_compile_features(onboard_instrument_cluster_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(onboard_instrument_cluster_lib)

qttutorial_add_app(onboard_instrument_cluster
    SOURCES
        src/main.cpp
        src/VehicleTelemetry.h
        src/VehicleTelemetry.cpp
    QT_LIBS Qt6::Quick Qt6::Qml onboard_instrument_cluster_lib
    QML_MODULE_URI QtTutorial.Automotive.Onboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/Gauge.qml
        src/qml/AdasStrip.qml
        src/qml/GearSelector.qml
        src/qml/TurnSignalArrow.qml
        src/qml/FuelGauge.qml
        src/qml/TirePressurePanel.qml
        src/qml/DrivingModeSelector.qml
)
