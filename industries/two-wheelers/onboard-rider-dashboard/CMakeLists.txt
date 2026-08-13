find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

add_library(two_wheelers_onboard_rider_dashboard_lib STATIC
    src/RideCycleSimulator.h
    src/RideCycleSimulator.cpp
)
target_include_directories(two_wheelers_onboard_rider_dashboard_lib PUBLIC src)
target_link_libraries(two_wheelers_onboard_rider_dashboard_lib PUBLIC Qt6::Core)
target_compile_features(two_wheelers_onboard_rider_dashboard_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(two_wheelers_onboard_rider_dashboard_lib)

qttutorial_add_app(two_wheelers_onboard_rider_dashboard
    SOURCES
        src/main.cpp
        src/RiderTelemetry.h
        src/RiderTelemetry.cpp
    QT_LIBS Qt6::Quick Qt6::Qml two_wheelers_onboard_rider_dashboard_lib
    QML_MODULE_URI QtTutorial.TwoWheelers.Onboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/Gauge.qml
        src/qml/LeanIndicator.qml
        src/qml/TyrePanel.qml
        src/qml/MotorcycleIcon.qml
        src/qml/HelmetIcon.qml
        src/qml/FuelGaugeIcon.qml
        src/qml/GearBadge.qml
        src/qml/FuelPanel.qml
        src/qml/RidingModeSelector.qml
)
