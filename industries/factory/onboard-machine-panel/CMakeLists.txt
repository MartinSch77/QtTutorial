find_package(Qt6 REQUIRED COMPONENTS Core Gui Qml Quick QuickControls2 StateMachine)

add_library(factory_machine_panel_lib STATIC
    src/MachineSimulator.h
    src/MachineSimulator.cpp
    src/MachineStateController.h
    src/MachineStateController.cpp
    src/EventLogModel.h
    src/EventLogModel.cpp
)
target_include_directories(factory_machine_panel_lib PUBLIC src)
target_link_libraries(factory_machine_panel_lib PUBLIC Qt6::Core Qt6::StateMachine)
target_compile_features(factory_machine_panel_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(factory_machine_panel_lib)

qttutorial_add_app(onboard_machine_panel
    SOURCES
        src/main.cpp
        src/MachinePanelApp.h
        src/MachinePanelApp.cpp
    QT_LIBS Qt6::Gui Qt6::Qml Qt6::Quick Qt6::QuickControls2 factory_machine_panel_lib
    QML_MODULE_URI Factory.OnboardMachinePanel
    QML_SOURCES qml/Main.qml
)
