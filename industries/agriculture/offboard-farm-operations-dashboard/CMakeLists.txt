find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(agriculture_offboard_farm_operations_dashboard_lib STATIC
    src/FieldOperationSimulator.h
    src/FieldOperationSimulator.cpp
    src/FieldOperationsModel.h
    src/FieldOperationsModel.cpp
    src/OperationHistoryStore.h
    src/OperationHistoryStore.cpp
)
target_include_directories(agriculture_offboard_farm_operations_dashboard_lib PUBLIC src)
target_link_libraries(agriculture_offboard_farm_operations_dashboard_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(agriculture_offboard_farm_operations_dashboard_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(agriculture_offboard_farm_operations_dashboard_lib)

qttutorial_add_app(agriculture_offboard_farm_operations_dashboard
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/FieldMapWidget.h
        src/FieldMapWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql agriculture_offboard_farm_operations_dashboard_lib
)
