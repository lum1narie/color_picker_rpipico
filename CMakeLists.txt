# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_COLOR_DIAGNOSTICS ON)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "/home/lum1narie/Development/pico/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(${CMAKE_SOURCE_DIR}/pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(color_picker C CXX ASM)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/src)

FIND_PACKAGE(Doxygen
  REQUIRED dot
  OPTIONAL_COMPONENTS mscgen dia)
IF(DOXYGEN_FOUND)
  SET(DOXYGEN_TARGET "doc")
  SET(DOXYGEN_CONF_FILE "doxygen.conf")

  SET(DOXYGEN_PROJECT_NAME ${PROEJCT_NAME})
  SET(DOXYGEN_PROJECT_NUMBER ${PROEJCT_NUMBER})
  SET(DOXYGEN_RECURSIVE yes)
  SET(DOXYGEN_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/doc)
  SET(DOXYGEN_EXTRACT_ALL YES)
  SET(DOXYGEN_EXTRACT_PRIVATE YES)
  SET(DOXYGEN_EXTRACT_STATIC YES)
  SET(DOXYGEN_EXTRACT_RECURSIVE YES)
  SET(DOXYGEN_UML_LOOK YES)
  SET(DOXYGEN_CALL_GRAPH YES)
  SET(DOXYGEN_CALLER_GRAPH YES)

  DOXYGEN_ADD_DOCS(${DOXYGEN_TARGET}
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/inc
  )
ELSE(DOXYGEN_FOUND)
  MESSAGE (WARNING "doxygen is NOT found")
ENDIF(DOXYGEN_FOUND)

add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/test)
