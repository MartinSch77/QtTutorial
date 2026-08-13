find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick)

add_library(onboard_cab_display_lib STATIC
    src/RouteProfile.h
    src/RouteProfile.cpp
    src/BrakingCurve.h
    src/BrakingCurve.cpp
    src/TrainSimulator.h
    src/TrainSimulator.cpp
)
target_include_directories(onboard_cab_display_lib PUBLIC src)
target_link_libraries(onboard_cab_display_lib PUBLIC Qt6::Core)
target_compile_features(onboard_cab_display_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(onboard_cab_display_lib)

qttutorial_add_app(onboard_cab_display
    SOURCES src/main.cpp
    QT_LIBS Qt6::Gui Qt6::Qml Qt6::Quick onboard_cab_display_lib
    QML_MODULE_URI Railway.OnboardCabDisplay
    QML_SOURCES
        qml/Main.qml
        qml/SpeedDial.qml
        qml/PlanningStrip.qml
        qml/TrainIcon.qml
        qml/SignalLampIcon.qml
        qml/RailIcon.qml
        qml/BrakeIcon.qml
        qml/DoorsIcon.qml
)
