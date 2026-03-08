cmake_minimum_required(VERSION 3.5)
project(system_programming VERSION 0.0.1)

set(target_sandbox sandbox)

# Standard Requirements:

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Download and update git submodules:

find_package(Git QUIET)
if (GIT_FOUND)
    message(STATUS "Submodule update")
    execute_process(
        COMMAND ${GIT_EXECUTABLE} submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        RESULT_VARIABLE GIT_SUBMOD_RESULT
    )
    if (NOT GIT_SUBMOD_RESULT EQUAL "0")
        message(FATAL_ERROR "git submodule update --init failed with ${GIT_SUBMOD_RESULT}, please checkout submodules")
    endif()
else()
    message(WARNING "Git is not found. It is required to update submodules.")
endif()

# Add directories from lib cmake projects
add_subdirectory(lib/core)

# Include cmake modules from git submodules:

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_local")

include(DetectOS)
include(CompilerOptions)
include(Logger)
include(SpCompilerOptions)

init_logger("[SYSTEM_PROGRAMMING]")

if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    log_fatal("MSVC compiler not supported")
endif()

# ---------------------------------------- Begin Options ---------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(SP_DEBUG OFF)
else()
    set(SP_DEBUG ON)
endif()

# Print Selected Options:

option(USE_ANSI_LOGGING "Use ANSI logging." OFF)

log_info("---------------------------------------------")
log_info("Version:                   ${PROJECT_VERSION}")
log_info("Platform:                  ${OS}")
log_info("Arch:                      ${CMAKE_SYSTEM_PROCESSOR}")
log_info("C++ Version:               ${CMAKE_CXX_STANDARD}")
log_info("Compiler:                  ${CMAKE_CXX_COMPILER_ID}")
log_info("Compiler Version:          ${CMAKE_CXX_COMPILER_VERSION}")
log_info("Debug:                     ${SP_DEBUG}")
log_info("Use Ansi Logging:          ${USE_ANSI_LOGGING}")

# ---------------------------------------- End Options -----------------------------------------------------------------

# ---------------------------------------- Begin Declare Source Files --------------------------------------------------

set(src_common

    src/core_init.cpp

    src/diagnostics/diagnostics.cpp
)

set(src_sandbox
    ${src_common}

    sandbox.cpp
)

# ---------------------------------------- End Declare Source Files ----------------------------------------------------

# ---------------------------------------- Begin Create Executables ----------------------------------------------------

macro(common_target_setup target)
    target_link_libraries(${target} PRIVATE core)

    target_include_directories(${target} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_definitions(${target} PRIVATE
        "USE_ANSI_LOGGING=$<BOOL:${USE_ANSI_LOGGING}>"
        "IS_DEBUG=$<BOOL:${SP_DEBUG}>"
    )

    sp_target_set_default_flags(${target} ${SP_DEBUG} false)
endmacro()

add_executable(${target_sandbox} ${src_sandbox})
common_target_setup(${target_sandbox})

log_info("---------------------------------------------")
