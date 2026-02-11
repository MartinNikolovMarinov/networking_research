include(CompilerOptions)

macro(networking_research_target_set_default_c_flags
    target
    is_debug
    save_temporary_files)

    set(common_flags -pthread)
    set(debug_flags "")
    set(release_flags "")

    if(${save_temporary_files})
        set(common_flags "${common_flags}" "-g" "-save-temps")
    endif()

    generate_common_c_flags(
        common_flags "${common_flags}"
        debug_flags "${debug_flags}"
        release_flags "${release_flags}"
    )

    # This apparently needs to be set after all other flags due to ordering.
    set(common_flags ${common_flags}
        -Wno-gnu-zero-variadic-macro-arguments # Suppress warning for " , ##__VA_ARGS__ " in variadic macros
    )

    if(${is_debug})
        target_compile_options(${target} PRIVATE ${common_flags} ${debug_flags})
    else()
        target_compile_options(${target} PRIVATE ${common_flags} ${release_flags})
    endif()

endmacro()
