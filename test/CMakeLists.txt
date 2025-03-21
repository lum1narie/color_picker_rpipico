project(test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

add_executable(cursor_test cursor_test.cpp)
target_compile_options(cursor_test PUBLIC -Wall -Wextra -fdiagnostics-color=always)

pico_set_program_name(cursor_test "cursor_test")
pico_set_program_version(cursor_test "0.1")

pico_enable_stdio_uart(cursor_test 1)
pico_enable_stdio_usb(cursor_test 1)

# Add the standard library to the build
target_link_libraries(cursor_test PRIVATE
  pico_stdlib
  hardware_spi
  pico_bootsel_via_double_reset
)

# Add any user requested libraries
target_link_libraries(cursor_test PRIVATE
  color
  LCD1in8
  color_picker
)

pico_add_extra_outputs(cursor_test)




add_executable(bg_color bg_color.cpp)
target_compile_options(bg_color PUBLIC -Wall -Wextra -fdiagnostics-color=always)

pico_set_program_name(bg_color "bg_color")
pico_set_program_version(bg_color "0.1")

pico_enable_stdio_uart(bg_color 1)
pico_enable_stdio_usb(bg_color 1)

# Add the standard library to the build
target_link_libraries(bg_color PRIVATE
  pico_stdlib
  hardware_spi
  pico_bootsel_via_double_reset
)

# Add any user requested libraries
target_link_libraries(bg_color PRIVATE
  color
  LCD1in8
  color_picker
)

pico_add_extra_outputs(bg_color)

