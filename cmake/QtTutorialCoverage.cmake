# Opt-in gcov/lcov coverage instrumentation, applied repo-wide via QTTUTORIAL_ENABLE_COVERAGE.
if(QTTUTORIAL_ENABLE_COVERAGE)
    if(NOT MSVC)
        add_compile_options(--coverage -O0 -g)
        add_link_options(--coverage)
    else()
        message(WARNING "QTTUTORIAL_ENABLE_COVERAGE has no effect with MSVC")
    endif()
endif()
