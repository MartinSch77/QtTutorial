find_package(Qt6 REQUIRED COMPONENTS Widgets Network)

add_library(networking_lib STATIC
    src/Protocol.h
    src/Protocol.cpp
    src/EchoServer.h
    src/EchoServer.cpp
)
target_include_directories(networking_lib PUBLIC src)
target_link_libraries(networking_lib PUBLIC Qt6::Core Qt6::Network)
target_compile_features(networking_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(networking_lib)

qttutorial_add_app(networking
    SOURCES src/main.cpp src/MainWindow.h src/MainWindow.cpp
    QT_LIBS Qt6::Widgets networking_lib
)
