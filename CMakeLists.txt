cmake_minimum_required(VERSION 3.21)

project(QtTutorial
    VERSION 0.1.0
    DESCRIPTION "A Qt 6 / C++23 framework tour and multi-industry showcase"
    HOMEPAGE_URL "https://github.com/MartinSch77/QtTutorial"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Build type" FORCE)
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

option(QTTUTORIAL_BUILD_FRAMEWORK_TOUR "Build the framework-tour modules" ON)
option(QTTUTORIAL_BUILD_INDUSTRIES "Build the industry showcase apps" ON)
option(QTTUTORIAL_BUILD_GAMES "Build the games (LAN multiplayer card/kicker games)" ON)
option(QTTUTORIAL_BUILD_SHOWCASES "Build the general-purpose visual showcase apps" ON)
option(QTTUTORIAL_BUILD_TESTS "Build unit tests" ON)
option(QTTUTORIAL_ENABLE_SANITIZERS "Build with ASan+UBSan" OFF)
option(QTTUTORIAL_ENABLE_TSAN "Build with ThreadSanitizer (mutually exclusive with QTTUTORIAL_ENABLE_SANITIZERS)" OFF)
option(QTTUTORIAL_ENABLE_COVERAGE "Build with --coverage instrumentation" OFF)
option(QTTUTORIAL_WARNINGS_AS_ERRORS "Treat compiler warnings as errors" OFF)

include(QtTutorialWarnings)
include(QtTutorialSanitizers)
include(QtTutorialCoverage)
include(QtTutorialTargets)

find_package(Qt6 6.4 REQUIRED COMPONENTS
    Core Gui Widgets Quick Qml QuickControls2
    Network Sql StateMachine SerialPort Concurrent
)
find_package(Qt6 COMPONENTS Graphs QUIET)
message(STATUS "Qt version found: ${Qt6_VERSION}")

if(QTTUTORIAL_BUILD_TESTS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)
endif()

if(QTTUTORIAL_BUILD_FRAMEWORK_TOUR)
    add_subdirectory(framework-tour)
endif()

if(QTTUTORIAL_BUILD_INDUSTRIES)
    add_subdirectory(industries)
endif()

if(QTTUTORIAL_BUILD_GAMES AND NOT QTTUTORIAL_BUILD_INDUSTRIES)
    # industries/CMakeLists.txt only descends into industries/games/ when it
    # runs; if industries as a whole are skipped but games specifically were
    # requested, add just that subdirectory here instead.
    add_subdirectory(industries/games games-only-build)
endif()

if(QTTUTORIAL_BUILD_SHOWCASES)
    add_subdirectory(showcases)
endif()

if(QTTUTORIAL_BUILD_TESTS)
    add_subdirectory(tests)
endif()
