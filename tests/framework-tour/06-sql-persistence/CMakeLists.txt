qttutorial_add_test(test_sql_persistence
    SOURCES test_sql_persistence.cpp
    LIB sql_persistence_lib
    QT_LIBS Qt6::Core Qt6::Sql
)
