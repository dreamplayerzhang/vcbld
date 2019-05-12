cmake_minimum_required(VERSION 3.10.0)

project(vcbld)
set (vcbld_VERSION_MAJOR 0)
set (vcbld_VERSION_MINOR 1)
set (vcbld_VERSION_PATCH 0)

configure_file (
  "${PROJECT_SOURCE_DIR}/src/vcbld.h.in"
  "${PROJECT_SOURCE_DIR}/src/vcbld.h"
)

add_subdirectory(src)

option(ENABLE_TESTS "Enable tests" OFF)
if (${ENABLE_TESTS})
    include(CTest)
    add_subdirectory(tests)
endif()

#to remove
set(CMAKE_PREFIX_PATH C:/Qt/5.11.2/msvc2017_64)

option(WITH_GUI "Add vcbld-gui" ON)
if (${WITH_GUI})
    set (vcbldgui_VERSION_MAJOR 0)
    set (vcbldgui_VERSION_MINOR 1)
    set (vcbldgui_VERSION_PATCH 0)

    configure_file (
  "${PROJECT_SOURCE_DIR}/vcbld-gui/vcbldgui.h.in"
  "${PROJECT_SOURCE_DIR}/vcbld-gui/vcbldgui.h"
    )
    add_subdirectory(vcbld-gui)
endif()

