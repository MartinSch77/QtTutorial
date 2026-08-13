find_package(Qt6 REQUIRED COMPONENTS Quick Qml)

add_library(news_ticker_lib STATIC
    src/Headline.h
    src/HeadlineGenerator.h
    src/HeadlineGenerator.cpp
)
target_include_directories(news_ticker_lib PUBLIC src)
target_link_libraries(news_ticker_lib PUBLIC Qt6::Core)
target_compile_features(news_ticker_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(news_ticker_lib)

qttutorial_add_app(news_ticker
    SOURCES
        src/main.cpp
        src/NewsFeedModel.h
        src/NewsFeedModel.cpp
    QT_LIBS Qt6::Quick Qt6::Qml news_ticker_lib
    QML_MODULE_URI QtTutorial.NewsTicker
    QML_SOURCES
        qml/Main.qml
        qml/HeadlineChip.qml
)
