find_package(Qt6 REQUIRED COMPONENTS Quick Qml QuickControls2)

add_library(qml_basics_lib STATIC
    src/IndicatorGenerator.h
    src/IndicatorGenerator.cpp
)
target_include_directories(qml_basics_lib PUBLIC src)
target_link_libraries(qml_basics_lib PUBLIC Qt6::Core)
target_compile_features(qml_basics_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(qml_basics_lib)

qttutorial_add_app(qml_quick_basics
    SOURCES
        src/main.cpp
        src/IndicatorListModel.h
        src/IndicatorListModel.cpp
        src/SystemStatusBackend.h
        src/SystemStatusBackend.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 qml_basics_lib
    QML_MODULE_URI QtTutorial.QmlQuickBasics
    QML_SOURCES
        qml/Main.qml
        qml/IndicatorTile.qml
)
