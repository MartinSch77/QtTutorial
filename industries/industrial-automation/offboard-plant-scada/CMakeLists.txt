find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)

add_library(plant_scada_lib STATIC
    src/ProcessPointSimulator.h
    src/ProcessPointSimulator.cpp
    src/AlarmEvaluator.h
    src/AlarmEvaluator.cpp
    src/Historian.h
    src/Historian.cpp
    src/TagTableModel.h
    src/TagTableModel.cpp
    src/PlantOverviewModel.h
    src/PlantOverviewModel.cpp
)
target_include_directories(plant_scada_lib PUBLIC src)
target_link_libraries(plant_scada_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(plant_scada_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(plant_scada_lib)

qttutorial_add_app(offboard_plant_scada
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/TrendWidget.h
        src/TrendWidget.cpp
        src/PlantOverviewWidget.h
        src/PlantOverviewWidget.cpp
    QT_LIBS Qt6::Widgets plant_scada_lib
)
