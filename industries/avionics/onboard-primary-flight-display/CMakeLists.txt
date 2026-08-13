find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(onboard_pfd_lib STATIC
    src/FlightDynamics.h
    src/FlightDynamics.cpp
    src/FlightDataSimulator.h
    src/FlightDataSimulator.cpp
)
target_include_directories(onboard_pfd_lib PUBLIC src)
target_link_libraries(onboard_pfd_lib PUBLIC Qt6::Core)
target_compile_features(onboard_pfd_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(onboard_pfd_lib)

qttutorial_add_app(onboard_primary_flight_display
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/AttitudeIndicator.h
        src/AttitudeIndicator.cpp
        src/VerticalTape.h
        src/VerticalTape.cpp
        src/HeadingIndicator.h
        src/HeadingIndicator.cpp
        src/VerticalSpeedIndicator.h
        src/VerticalSpeedIndicator.cpp
    QT_LIBS Qt6::Widgets onboard_pfd_lib
)
