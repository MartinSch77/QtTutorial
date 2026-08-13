find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

add_library(mining_onboard_haul_truck_console_lib STATIC
    src/HaulCycleSimulator.h
    src/HaulCycleSimulator.cpp
)
target_include_directories(mining_onboard_haul_truck_console_lib PUBLIC src)
target_link_libraries(mining_onboard_haul_truck_console_lib PUBLIC Qt6::Core)
target_compile_features(mining_onboard_haul_truck_console_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(mining_onboard_haul_truck_console_lib)

qttutorial_add_app(mining_onboard_haul_truck_console
    SOURCES
        src/main.cpp
        src/HaulTruckTelemetry.h
        src/HaulTruckTelemetry.cpp
    QT_LIBS Qt6::Quick Qt6::Qml mining_onboard_haul_truck_console_lib
    QML_MODULE_URI QtTutorial.Mining.Onboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/Gauge.qml
        src/qml/HaulStateStrip.qml
        src/qml/TyrePressureGrid.qml
)
