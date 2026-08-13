find_package(Qt6 REQUIRED COMPONENTS Widgets StateMachine)

add_library(state_machine_lib STATIC
    src/GuardedSignalTransition.h
    src/DeviceController.h
    src/DeviceController.cpp
)
target_include_directories(state_machine_lib PUBLIC src)
target_link_libraries(state_machine_lib PUBLIC Qt6::Core Qt6::StateMachine)
target_compile_features(state_machine_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(state_machine_lib)

qttutorial_add_app(state_machine
    SOURCES src/main.cpp src/MainWindow.h src/MainWindow.cpp
    QT_LIBS Qt6::Widgets state_machine_lib
)
