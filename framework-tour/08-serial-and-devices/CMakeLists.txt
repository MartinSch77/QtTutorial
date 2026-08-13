find_package(Qt6 REQUIRED COMPONENTS Widgets SerialPort Network)

add_library(serial_and_devices_lib STATIC
    src/FrameCodec.h
    src/FrameCodec.cpp
    src/FrameParser.h
    src/FrameParser.cpp
    src/DeviceLink.h
    src/DeviceLink.cpp
    src/SimulatedSerialLink.h
    src/SimulatedSerialLink.cpp
)
target_include_directories(serial_and_devices_lib PUBLIC src)
target_link_libraries(serial_and_devices_lib PUBLIC Qt6::Core Qt6::Network Qt6::SerialPort)
target_compile_features(serial_and_devices_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(serial_and_devices_lib)

qttutorial_add_app(serial_and_devices
    SOURCES src/main.cpp src/MainWindow.h src/MainWindow.cpp
    QT_LIBS Qt6::Widgets serial_and_devices_lib
)
