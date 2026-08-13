find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(mining_offboard_pit_operations_dashboard_lib STATIC
    src/HaulFleetSimulator.h
    src/HaulFleetSimulator.cpp
    src/PitFleetModel.h
    src/PitFleetModel.cpp
    src/HaulHistoryStore.h
    src/HaulHistoryStore.cpp
    src/PitIcons.h
    src/PitIcons.cpp
)
target_include_directories(mining_offboard_pit_operations_dashboard_lib PUBLIC src)
# Qt6::Gui is already an implicit dependency of this app's Qt6::Widgets - not
# a new Qt module - and is needed here for the QIcon/QPainter-based warning
# and truck glyphs shared by PitFleetModel (table decoration) and
# PitMapWidget (pit-map view).
target_link_libraries(mining_offboard_pit_operations_dashboard_lib PUBLIC Qt6::Core Qt6::Gui Qt6::Sql)
target_compile_features(mining_offboard_pit_operations_dashboard_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(mining_offboard_pit_operations_dashboard_lib)

qttutorial_add_app(mining_offboard_pit_operations_dashboard
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/ProductionTrendWidget.h
        src/ProductionTrendWidget.cpp
        src/PitMapWidget.h
        src/PitMapWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql mining_offboard_pit_operations_dashboard_lib
)
