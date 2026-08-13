find_package(Qt6 REQUIRED COMPONENTS Quick Qml QuickControls2 StateMachine Svg)
find_package(Qt6 COMPONENTS Graphs QUIET)
find_package(Qt6 COMPONENTS WebSockets QUIET)

add_library(stock_tracker_lib STATIC
    src/PriceSimulator.h
    src/PriceSimulator.cpp
    src/GuardedTransition.h
    src/OrderLifecycle.h
    src/OrderLifecycle.cpp
)
target_include_directories(stock_tracker_lib PUBLIC src)
target_link_libraries(stock_tracker_lib PUBLIC Qt6::Core Qt6::StateMachine)
target_compile_features(stock_tracker_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(stock_tracker_lib)

# Qt WebSockets is only used for the live market-data feed (WebSocketMarketFeed);
# everything else (price simulation, the order state machine) is independent of
# it. Guarded exactly like framework-tour/09-latest-qt-release-features guards
# Qt Graphs, except that here only this one optional piece is skipped rather
# than the whole module, so the app still builds and runs on Qt 6.4.2.
if(TARGET Qt6::WebSockets)
    target_sources(stock_tracker_lib PRIVATE
        src/WebSocketMarketFeed.h
        src/WebSocketMarketFeed.cpp
    )
    target_link_libraries(stock_tracker_lib PUBLIC Qt6::WebSockets Qt6::Network)
    target_compile_definitions(stock_tracker_lib PUBLIC STOCK_TRACKER_HAVE_WEBSOCKETS)
    message(STATUS "showcases/stock-tracker: Qt6::WebSockets found - using a real local QWebSocketServer/QWebSocket feed")
else()
    message(STATUS "showcases/stock-tracker: Qt6::WebSockets not found (needs Qt >= 6.5) - "
                    "falling back to a direct in-process feed, see StockTrackerBackend::feedMode()")
endif()

qt6_add_resources(STOCK_TRACKER_ASSETS resources/assets.qrc)

set(STOCK_TRACKER_QML
    qml/Main.qml
    qml/TickerList.qml
    qml/DetailPanel.qml
    qml/OrderPanel.qml
    qml/OrderStateBadge.qml
    qml/PriceChartFallback.qml
)

# PriceChartGraphs.qml does `import QtGraphs`; it is only added to the QML
# module's sources (and therefore only ever compiled/bundled) when Qt6::Graphs
# was actually found, so an older Qt without the Graphs module never sees an
# unresolvable import. See showcases/stock-tracker/qml/DetailPanel.qml, which
# Loaders it only when StockTrackerBackend::graphsAvailable() is true.
if(TARGET Qt6::Graphs)
    list(APPEND STOCK_TRACKER_QML qml/PriceChartGraphs.qml)
endif()

qttutorial_add_app(stock_tracker
    SOURCES
        src/main.cpp
        src/TickerListModel.h
        src/TickerListModel.cpp
        src/StockTrackerBackend.h
        src/StockTrackerBackend.cpp
        ${STOCK_TRACKER_ASSETS}
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 Qt6::Svg stock_tracker_lib
    QML_MODULE_URI QtTutorial.StockTracker
    QML_SOURCES ${STOCK_TRACKER_QML}
)

if(TARGET Qt6::Graphs)
    target_link_libraries(stock_tracker PRIVATE Qt6::Graphs)
    target_compile_definitions(stock_tracker PRIVATE STOCK_TRACKER_HAVE_GRAPHS)
    message(STATUS "showcases/stock-tracker: Qt6::Graphs found - interactive Qt Graphs price chart enabled")
else()
    message(STATUS "showcases/stock-tracker: Qt6::Graphs not found (needs Qt >= 6.9) - "
                    "using the QML Shapes fallback chart (qml/PriceChartFallback.qml) only")
endif()
