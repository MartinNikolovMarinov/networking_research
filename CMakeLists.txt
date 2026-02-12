cmake_minimum_required(VERSION 3.21)
project(networking-research VERSION 0.0.1 LANGUAGES C CXX)

set(target_sandbox sandbox)
set(target_tests tests)
set(target_vm_to_vm_lab vm_to_vm_lab)

# Standard Requirements:

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generates compile_commands.json

# Add directories from lib cmake projects

# TODO: Is this necessary?
# option(CORE_LIBRARY_SHARED "Build core as a shared library." OFF)
# option(CORE_ASSERT_ENABLED "Enable Asserts in corelib" OFF)
# add_subdirectory(lib/core)

# Include cmake modules from:

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake_local")

include(CompilerOptions)
include(Logger)
include(NetworkingResearchClangOptions)

init_logger("[NETWORKING_RESEARCH]")

# ---------------------------------------- Begin Options ---------------------------------------------------------------

if(NOT CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE STREQUAL "Release")
    set(NETWORKING_RESEARCH_DEBUG OFF)
else()
    set(NETWORKING_RESEARCH_DEBUG ON)
endif()

# Print Selected Options:

log_info("---------------------------------------------")
log_info("Version:           ${PROJECT_VERSION}")
log_info("Platform:          ${OS}")
log_info("Arch:              ${CMAKE_SYSTEM_PROCESSOR}")
log_info("C Version:         ${CMAKE_C_STANDARD}")
log_info("C Compiler:        ${CMAKE_C_COMPILER_ID}")
log_info("Debug:             ${NETWORKING_RESEARCH_DEBUG}")
log_info("---------------------------------------------")

# ---------------------------------------- End Options -----------------------------------------------------------------

# ---------------------------------------- Begin Declare Source Files --------------------------------------------------

set(src_common
    src/nr_bits.c
    src/nr_client.c
    src/nr_logger.c
    src/nr_time.c
)

set(src_sandbox
    ${src_common}

    sandbox.c
)

set(src_tests
    ${src_common}

    tests/test_runner.c
    tests/tests_main.c
    tests/t-index.c
    tests/t-bits.c
    tests/t-logger.c
)

set(src_vm_to_vm_lab
    ${src_common}

    projects/vm_to_vm_lab/nr_virtual_interface.c
    projects/vm_to_vm_lab/vm_to_vm_lab_main.c
)

# ---------------------------------------- End Declare Source Files ----------------------------------------------------

# ---------------------------------------- Begin Create Executable -----------------------------------------------------

macro(common_target_setup target)
    target_include_directories(${target} PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
    )

    target_compile_definitions(${target} PUBLIC
        "IS_DEBUG=$<BOOL:${NETWORKING_RESEARCH_DEBUG}>"
    )

    networking_research_target_set_default_c_flags(${target} ${NETWORKING_RESEARCH_DEBUG} false)
endmacro()

add_executable(${target_sandbox} ${src_sandbox})
common_target_setup(${target_sandbox})

add_executable(${target_tests} ${src_tests})
common_target_setup(${target_tests})
target_include_directories(${target_tests} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/tests
)

add_executable(${target_vm_to_vm_lab} ${src_vm_to_vm_lab})
common_target_setup(${target_vm_to_vm_lab})

# ---------------------------------------- END Create Executable -------------------------------------------------------
