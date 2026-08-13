# Conventions shared by every framework-tour module and industry example.
#
# qttutorial_add_app(<target> SOURCES ... [QT_LIBS ...] [QML_MODULE_URI <uri> QML_SOURCES ...])
function(qttutorial_add_app target)
    set(options "")
    set(oneValueArgs QML_MODULE_URI)
    set(multiValueArgs SOURCES QT_LIBS QML_SOURCES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${target} ${ARG_SOURCES})
    target_link_libraries(${target} PRIVATE ${ARG_QT_LIBS})
    target_compile_features(${target} PRIVATE cxx_std_23)
    qttutorial_set_warnings(${target})

    if(ARG_QML_MODULE_URI)
        qt_add_qml_module(${target}
            URI ${ARG_QML_MODULE_URI}
            VERSION 1.0
            QML_FILES ${ARG_QML_SOURCES}
        )
    endif()
endfunction()

# qttutorial_add_test(<target> SOURCES ... LIB <libraryUnderTest> [QT_LIBS ...])
function(qttutorial_add_test target)
    set(options "")
    set(oneValueArgs LIB)
    set(multiValueArgs SOURCES QT_LIBS)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    add_executable(${target} ${ARG_SOURCES})
    target_link_libraries(${target} PRIVATE Qt6::Test ${ARG_LIB} ${ARG_QT_LIBS})
    target_compile_features(${target} PRIVATE cxx_std_23)
    add_test(NAME ${target} COMMAND ${target})
    set_tests_properties(${target} PROPERTIES LABELS "qttutorial")
endfunction()
