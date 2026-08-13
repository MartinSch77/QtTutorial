find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_library(widgets_basics_lib STATIC
    src/UnitConversion.h
    src/UnitConversion.cpp
    src/UnitConverterModel.h
    src/UnitConverterModel.cpp
)
target_include_directories(widgets_basics_lib PUBLIC src)
target_link_libraries(widgets_basics_lib PUBLIC Qt6::Core)
target_compile_features(widgets_basics_lib PUBLIC cxx_std_23)
qttutorial_set_warnings(widgets_basics_lib)

qttutorial_add_app(widgets_basics
    SOURCES src/main.cpp src/MainWindow.h src/MainWindow.cpp
    QT_LIBS Qt6::Widgets widgets_basics_lib
)
