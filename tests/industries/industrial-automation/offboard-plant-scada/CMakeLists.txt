qttutorial_add_test(test_plant_scada_logic
    SOURCES test_plant_scada_logic.cpp
    LIB plant_scada_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
