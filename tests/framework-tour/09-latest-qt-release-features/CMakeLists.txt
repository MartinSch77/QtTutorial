if(NOT TARGET graphs_showcase_lib)
    return()
endif()

qttutorial_add_test(test_telemetry_generator
    SOURCES test_telemetry_generator.cpp
    LIB graphs_showcase_lib
    QT_LIBS Qt6::Core
)
