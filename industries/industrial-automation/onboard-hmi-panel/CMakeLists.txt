find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick StateMachine)

add_library(onboard_hmi_panel_lib STATIC
    src/ProcessSimulator.h
    src/ProcessSimulator.cpp
    src/PumpController.h
    src/PumpController.cpp
    src/AlarmLogModel.h
    src/AlarmLogModel.cpp
)
target_include_directories(onboard_hmi_panel_lib PUBLIC src)
target_link_libraries(onboard_hmi_panel_lib PUBLIC Qt6::Core Qt6::StateMachine)
target_compile_features(onboard_hmi_panel_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(onboard_hmi_panel_lib)

qttutorial_add_app(onboard_hmi_panel
    SOURCES
        src/main.cpp
        src/HmiApp.h
        src/HmiApp.cpp
    QT_LIBS Qt6::Gui Qt6::Qml Qt6::Quick onboard_hmi_panel_lib
    QML_MODULE_URI IndustrialAutomation.OnboardHmiPanel
    QML_SOURCES qml/Main.qml
)
