cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Pull in Raspberry Pi Pico SDK (must be before project)
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(color C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_library(color STATIC
  color.cpp
)

target_compile_options(color PUBLIC -Wall -Wextra -fdiagnostics-color=always)

target_include_directories(color PUBLIC
  ${PROJECT_SOURCE_DIR}
)


target_link_libraries(color PRIVATE
  pico_stdlib
)
