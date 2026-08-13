if(NOT TARGET quick3d_physics_layout_lib)
    return()
endif()

qttutorial_add_test(test_drop_layout
    SOURCES test_drop_layout.cpp
    LIB quick3d_physics_layout_lib
    QT_LIBS Qt6::Core
)
