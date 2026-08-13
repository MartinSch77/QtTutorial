# Opt-in ASan+UBSan instrumentation, applied repo-wide via QTTUTORIAL_ENABLE_SANITIZERS.
if(QTTUTORIAL_ENABLE_SANITIZERS)
    if(MSVC)
        add_compile_options(/fsanitize=address)
    else()
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()
