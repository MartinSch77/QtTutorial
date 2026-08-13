qttutorial_add_test(test_concurrency_async
    SOURCES test_concurrency_async.cpp
    LIB concurrency_async_lib
    QT_LIBS Qt6::Core Qt6::Concurrent
)
