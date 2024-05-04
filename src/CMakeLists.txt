# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1
add_executable(color_picker color_picker.cpp)
set_property(TARGET color_picker PROPERTY
  ENABLE_EXPORTS 1
)
target_compile_options(color_picker PUBLIC -Wall -Wextra -fdiagnostics-color=always)

target_sources(color_picker PUBLIC
  display.cpp
)

pico_set_program_name(color_picker "color_picker")
pico_set_program_version(color_picker "0.1")

pico_enable_stdio_uart(color_picker 1)
pico_enable_stdio_usb(color_picker 1)

# Add the standard library to the build
target_link_libraries(color_picker PRIVATE
  pico_stdlib
  hardware_spi
  pico_bootsel_via_double_reset
)

# Add the standard include files to the build
target_include_directories(color_picker PUBLIC
  ${PROJECT_SOURCE_DIR}/inc
)

# Add any user requested libraries
target_link_libraries(color_picker PRIVATE
  color
  LCD1in8
)

pico_add_extra_outputs(color_picker)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/LCD1in8)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/color)
