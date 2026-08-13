qttutorial_add_test(test_thermostat_model
    SOURCES test_thermostat_model.cpp
    LIB onboard_wall_panel_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_room_registry
    SOURCES test_room_registry.cpp
    LIB onboard_wall_panel_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_lock_registry
    SOURCES test_lock_registry.cpp
    LIB onboard_wall_panel_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_security_center
    SOURCES test_security_center.cpp
    LIB onboard_wall_panel_lib
    QT_LIBS Qt6::Core
)

qttutorial_add_test(test_scene_registry
    SOURCES test_scene_registry.cpp
    LIB onboard_wall_panel_lib
    QT_LIBS Qt6::Core
)
