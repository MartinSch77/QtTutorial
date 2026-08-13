# Opt-in ASan+UBSan instrumentation, applied repo-wide via QTTUTORIAL_ENABLE_SANITIZERS.
if(QTTUTORIAL_ENABLE_SANITIZERS)
    if(MSVC)
        add_compile_options(/fsanitize=address)
    else()
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# Opt-in ThreadSanitizer instrumentation, applied repo-wide via
# QTTUTORIAL_ENABLE_TSAN. Kept as a second, separate option (not a value of
# QTTUTORIAL_ENABLE_SANITIZERS) because ASan and TSan instrument the
# allocator/runtime in incompatible ways -- `-fsanitize=address` and
# `-fsanitize=thread` cannot be combined in the same binary (this is a
# documented Clang/GCC restriction, not a limitation specific to this repo).
# See .github/workflows/sanitizers.yml for the CI job that builds with this
# ON; it is meaningful now that framework-tour/05-concurrency-async uses
# std::jthread and industries/games/kicker runs a real-time simulation loop with its
# own worker thread(s).
if(QTTUTORIAL_ENABLE_TSAN AND QTTUTORIAL_ENABLE_SANITIZERS)
    message(FATAL_ERROR
        "QTTUTORIAL_ENABLE_TSAN and QTTUTORIAL_ENABLE_SANITIZERS (ASan+UBSan) "
        "are mutually exclusive -- enable only one per build/configure.")
endif()

if(QTTUTORIAL_ENABLE_TSAN)
    if(MSVC)
        message(WARNING "QTTUTORIAL_ENABLE_TSAN has no effect with MSVC (no ThreadSanitizer support).")
    else()
        add_compile_options(-fsanitize=thread -fno-omit-frame-pointer -g)
        add_link_options(-fsanitize=thread)
    endif()
endif()
