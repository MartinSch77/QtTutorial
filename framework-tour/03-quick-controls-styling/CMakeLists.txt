find_package(Qt6 REQUIRED COMPONENTS Quick Qml QuickControls2)

add_library(quick_controls_styling_lib STATIC
    src/StyleCatalog.h
    src/StyleCatalog.cpp
)
target_include_directories(quick_controls_styling_lib PUBLIC src)
target_link_libraries(quick_controls_styling_lib PUBLIC Qt6::Core)
target_compile_features(quick_controls_styling_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(quick_controls_styling_lib)

qttutorial_add_app(quick_controls_styling
    SOURCES src/main.cpp src/StyleCatalogModel.h src/StyleCatalogModel.cpp
    QT_LIBS Qt6::Quick Qt6::Qml Qt6::QuickControls2 quick_controls_styling_lib
    QML_MODULE_URI QtTutorial.QuickControlsStyling
    QML_SOURCES qml/Main.qml
)
