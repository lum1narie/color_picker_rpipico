cmake_minimum_required(VERSION 3.13)

# include(../../pico_sdk_import.cmake)
# pico_sdk_init()

add_library(LCD1in8 STATIC
  LCD.cpp
)

add_subdirectory(
  ${CMAKE_CURRENT_LIST_DIR}/Fonts
)

target_include_directories(LCD1in8 PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(LCD1in8 PUBLIC
  Fonts
)

target_link_libraries(LCD1in8 PRIVATE
  pico_stdlib
  hardware_spi
)

