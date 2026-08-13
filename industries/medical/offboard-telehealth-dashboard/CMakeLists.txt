find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(offboard_telehealth_dashboard_lib STATIC
    src/PatientVitalsSimulator.h
    src/PatientVitalsSimulator.cpp
    src/PatientListModel.h
    src/PatientListModel.cpp
    src/VitalsHistoryStore.h
    src/VitalsHistoryStore.cpp
)
target_include_directories(offboard_telehealth_dashboard_lib PUBLIC src)
target_link_libraries(offboard_telehealth_dashboard_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(offboard_telehealth_dashboard_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(offboard_telehealth_dashboard_lib)

qttutorial_add_app(offboard_telehealth_dashboard
    SOURCES
        src/main.cpp
        src/MainWindow.h
        src/MainWindow.cpp
        src/SparklineDelegate.h
        src/SparklineDelegate.cpp
        src/IconPainter.h
        src/IconPainter.cpp
        src/PatientOverviewWidget.h
        src/PatientOverviewWidget.cpp
        src/TrendChartWidget.h
        src/TrendChartWidget.cpp
    QT_LIBS Qt6::Widgets Qt6::Sql offboard_telehealth_dashboard_lib
)
