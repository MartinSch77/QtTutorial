find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

add_library(fleet_ops_lib STATIC
    src/DelayCalculator.h
    src/DelayCalculator.cpp
    src/FleetSimulator.h
    src/FleetSimulator.cpp
    src/RunHistoryStore.h
    src/RunHistoryStore.cpp
    src/FleetTableModel.h
    src/FleetTableModel.cpp
)
target_include_directories(fleet_ops_lib PUBLIC src)
target_link_libraries(fleet_ops_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(fleet_ops_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(fleet_ops_lib)

qttutorial_add_app(offboard_fleet_ops_center
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/DelayTrendWidget.h
        src/DelayTrendWidget.cpp
    QT_LIBS Qt6::Widgets fleet_ops_lib
)
