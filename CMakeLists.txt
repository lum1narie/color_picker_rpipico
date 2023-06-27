# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "/home/lum1narie/Development/pico/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(color_picker C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(color_picker color_picker.cpp )
target_compile_options(color_picker PUBLIC -Wall -Wextra)
target_sources(color_picker PRIVATE
  color.cpp)

pico_set_program_name(color_picker "color_picker")
pico_set_program_version(color_picker "0.1")

pico_enable_stdio_uart(color_picker 1)
pico_enable_stdio_usb(color_picker 1)

add_subdirectory(
  ${CMAKE_CURRENT_LIST_DIR}/lib/LCD1in8
)

# Add the standard library to the build
target_link_libraries(color_picker PRIVATE
  pico_stdlib
)

# Add the standard include files to the build
target_include_directories(color_picker PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
)


# Add any user requested libraries
target_link_libraries(color_picker PRIVATE
  LCD1in8
  hardware_spi
  pico_bootsel_via_double_reset
)

pico_add_extra_outputs(color_picker)

