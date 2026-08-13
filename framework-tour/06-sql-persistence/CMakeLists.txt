find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)

add_library(sql_persistence_lib STATIC
    src/TaskRepository.h
    src/TaskRepository.cpp
)
target_include_directories(sql_persistence_lib PUBLIC src)
target_link_libraries(sql_persistence_lib PUBLIC Qt6::Core Qt6::Sql)
target_compile_features(sql_persistence_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(sql_persistence_lib)

qttutorial_add_app(sql_persistence
    SOURCES src/main.cpp src/MainWindow.h src/MainWindow.cpp
    QT_LIBS Qt6::Widgets sql_persistence_lib
)
