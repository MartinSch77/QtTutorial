find_package(Qt6 REQUIRED COMPONENTS Network)

add_library(games_common_lib STATIC
    src/TableMessage.h
    src/TableMessage.cpp
    src/TableServer.h
    src/TableServer.cpp
    src/TableClient.h
    src/TableClient.cpp
    src/LanBeacon.h
    src/LanBeacon.cpp
    src/LanAdvertiser.h
    src/LanAdvertiser.cpp
    src/LanBrowser.h
    src/LanBrowser.cpp
)
target_include_directories(games_common_lib PUBLIC src)
target_link_libraries(games_common_lib PUBLIC Qt6::Core Qt6::Network)
target_compile_features(games_common_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(games_common_lib)
