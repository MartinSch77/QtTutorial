find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(mining_offboard_pit_operations_dashboard_lib STATIC
    src/HaulFleetSimulator.h
    src/HaulFleetSimulator.cpp
    src/PitFleetModel.h
    src/PitFleetModel.cpp
    src/HaulHistoryStore.h
    src/HaulHistoryStore.cpp
)
target_include_directories(mining_offboard_pit_operations_dashboard_lib PUBLIC src)
target_link_libraries(mining_offboard_pit_operations_dashboard_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(mining_offboard_pit_operations_dashboard_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(mining_offboard_pit_operations_dashboard_lib)

qttutorial_add_app(mining_offboard_pit_operations_dashboard
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/ProductionTrendWidget.h
        src/ProductionTrendWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql mining_offboard_pit_operations_dashboard_lib
)
