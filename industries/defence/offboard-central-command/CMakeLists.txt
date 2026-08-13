find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(central_command_lib STATIC
    src/Asset.h
    src/Asset.cpp
    src/AlertLog.h
    src/AlertLog.cpp
    src/FleetSimulator.h
    src/FleetSimulator.cpp
    src/AssetHistoryStore.h
    src/AssetHistoryStore.cpp
)
target_include_directories(central_command_lib PUBLIC src)
target_link_libraries(central_command_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(central_command_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(central_command_lib)

qttutorial_add_app(offboard_central_command
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/AssetTableModel.h
        src/AssetTableModel.cpp
        src/TacticalMapWidget.h
        src/TacticalMapWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql central_command_lib
)
