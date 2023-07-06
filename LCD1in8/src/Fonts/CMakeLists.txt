cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

target_sources(LCD1in8 PRIVATE
  font8.c
  font12.c
  font16.c
  font20.c
  font24.c
)

target_include_directories(LCD1in8 PRIVATE
  ${PROJECT_SOURCE_DIR}/inc
)
