find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

add_library(onboard_patient_monitor_lib STATIC
    src/EcgWaveformGenerator.h
    src/EcgWaveformGenerator.cpp
    src/VitalsSimulator.h
    src/VitalsSimulator.cpp
    src/AlarmStateMachine.h
    src/AlarmStateMachine.cpp
)
target_include_directories(onboard_patient_monitor_lib PUBLIC src)
target_link_libraries(onboard_patient_monitor_lib PUBLIC Qt6::Core)
target_compile_features(onboard_patient_monitor_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(onboard_patient_monitor_lib)

qttutorial_add_app(onboard_patient_monitor
    SOURCES
        src/main.cpp
        src/PatientMonitor.h
        src/PatientMonitor.cpp
    QT_LIBS Qt6::Quick Qt6::Qml onboard_patient_monitor_lib
    QML_MODULE_URI QtTutorial.Medical.Onboard
    QML_SOURCES
        src/qml/Main.qml
        src/qml/EcgCanvas.qml
        src/qml/VitalTile.qml
)
