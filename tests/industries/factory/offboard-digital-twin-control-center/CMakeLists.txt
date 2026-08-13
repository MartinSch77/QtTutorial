# SPDX-License-Identifier: GPL-3.0-or-later
if(NOT TARGET factory_control_center_lib)
    return()
endif()

qttutorial_add_test(test_machine_telemetry_simulator
    SOURCES test_machine_telemetry_simulator.cpp
    LIB factory_control_center_lib
)

qttutorial_add_test(test_machine_list_model
    SOURCES test_machine_list_model.cpp
    LIB factory_control_center_lib
)

qttutorial_add_test(test_maintenance_workflow
    SOURCES test_maintenance_workflow.cpp
    LIB factory_control_center_lib
)

qttutorial_add_test(test_demo_conductor
    SOURCES test_demo_conductor.cpp
    LIB factory_control_center_lib
)

qttutorial_add_test(test_alarm_audio_controller
    SOURCES test_alarm_audio_controller.cpp
    LIB factory_control_center_lib
)
