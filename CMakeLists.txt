cmake_minimum_required(VERSION 3.10.0)

project(vcbld)
set (vcbld_VERSION_MAJOR 0)
set (vcbld_VERSION_MINOR 1)
set (vcbld_VERSION_PATCH 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/vcbldConfig.h.in"
  "${PROJECT_BINARY_DIR}/vcbldConfig.h"
  )

include_directories(${PROJECT_BINARY_DIR})

add_subdirectory(src)

option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    include(CTest)
    add_subdirectory(tests)
endif()

