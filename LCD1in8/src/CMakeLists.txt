cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(
  ${CMAKE_CURRENT_LIST_DIR}/Fonts
)

add_library(LCD1in8 STATIC
  LCD.cpp
  LCD_buffer.cpp
)
target_include_directories(LCD1in8 PUBLIC
  ${PROJECT_SOURCE_DIR}/inc
)

target_link_libraries(LCD1in8 PRIVATE
  Fonts
)
