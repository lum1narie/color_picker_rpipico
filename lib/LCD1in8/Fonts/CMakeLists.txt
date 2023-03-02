cmake_minimum_required(VERSION 3.13)

add_library(Fonts STATIC
  font12.c
  font16.c
  font20.c
  font24.c
  font8.c
)

target_include_directories(Fonts PUBLIC
  ${CMAKE_CURRENT_LIST_DIR}
)
