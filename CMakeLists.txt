# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)

# == DO NEVER EDIT THE NEXT LINES for Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()

set(sdkVersion 2.0.0)
set(toolchainVersion 13_2_Rel1)
set(picotoolVersion 2.0.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)

if(EXISTS ${picoVscode})
    include(${picoVscode})
endif()

# ====================================================================================
set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

project(pico_usb_hid_forwarding C CXX ASM)
set(target_name ${PROJECT_NAME})
add_executable(${target_name})

set(PICO_CXX_ENABLE_EXCEPTIONS 1)
set(PICO_CXX_ENABLE_RTTI 1)

if(DEFINED ENV{PICO_DEBUG} OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    set(PICO_DEOPTIMIZED_DEBUG 1)
endif()

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

target_sources(${target_name} PRIVATE
    src/main.cpp
)

pico_set_program_name(${target_name} "${target_name}")
pico_set_program_version(${target_name} "0.1")

# Modify the below lines to enable/disable output over UART/USB
pico_enable_stdio_uart(${target_name} 0)
pico_enable_stdio_usb(${target_name} 0)

target_link_options(${target_name} PRIVATE -Xlinker --print-memory-usage)

if(DEFINED ENV{PICO_DEBUG} OR CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -Og)
ELSE()
    target_compile_options(${target_name} PRIVATE -Wall -Wextra -O3)
ENDIF()

# Add the standard library to the build
target_link_libraries(${target_name}
    pico_stdlib)

# Add the standard include files to the build
target_include_directories(${target_name} PRIVATE
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

pico_add_extra_outputs(${target_name})
