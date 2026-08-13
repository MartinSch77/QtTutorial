find_package(Qt6 REQUIRED COMPONENTS Widgets Concurrent)

add_library(concurrency_async_lib STATIC
    src/SensorData.h
    src/SensorData.cpp
    src/StatsCompute.h
    src/StatsCompute.cpp
    src/QtConcurrentPipeline.h
    src/QtConcurrentPipeline.cpp
    src/JThreadWorker.h
    src/JThreadWorker.cpp
)
target_include_directories(concurrency_async_lib PUBLIC src)
target_link_libraries(concurrency_async_lib PUBLIC Qt6::Core Qt6::Concurrent)
target_compile_features(concurrency_async_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(concurrency_async_lib)

qttutorial_add_app(concurrency_async
    SOURCES src/main.cpp src/MainWindow.h src/MainWindow.cpp
    QT_LIBS Qt6::Widgets concurrency_async_lib
)
