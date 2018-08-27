set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/release)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ~/vcpkg)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "-Weverything")

# Set some Win32 Specific Settings
IF(WIN32)
SET(GUI_TYPE WIN32)
ENDIF(WIN32)

# Set some Apple MacOS Specific settings
IF (APPLE)
SET(GUI_TYPE MACOSX_BUNDLE)
ENDIF (APPLE)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

if(CMAKE_COMPILER_IS_GNUXX OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(GCC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
        message(FATAL_ERROR
"Building vcbld requires support for the C++ Filesystem TS.
Apple clang versions 9 and below do not have support for it.
Please install gcc6 or newer from homebrew (brew install gcc6 or brew install llvm).")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "[Cc]lang")
    set(CLANG 1)
    if(APPLE)
        set(CMAKE_EXE_LINKER_FLAGS -L/usr/local/opt/llvm/lib) #if clang was installed via homebrew on MacOS X
    endif()
elseif(MSVC)
    add_compile_options(/std:c++latest)
else()
    message(FATAL_ERROR "Unknown compiler: ${CMAKE_CXX_COMPILER_ID}")
endif()

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
	set(CMAKE_BUILD_TYPE Release)
endif()

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "Prevented in-tree build. Please create a build directory outside of the source directory and call cmake from there.")
endif()

set(CMAKE_PREFIX_PATH ~/Qt/5.11.1/clang_64)

set(GUI_FILES "main.cpp" "mainwindow.cpp" "mainwindow.ui" "remove.cpp" "remove.ui" "add.cpp" "add.ui")

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

find_package(Qt5 COMPONENTS Core Gui Widgets CONFIG REQUIRED)

add_executable(vcbld-gui ${GUI_FILES})
target_link_libraries(vcbld-gui PRIVATE vcbldlib Qt5::Core Qt5::Gui Qt5::Widgets)

if(GCC)
    target_link_libraries(vcbld-gui PRIVATE stdc++fs)
elseif(CLANG)
    target_link_libraries(vcbld-gui PRIVATE c++experimental)
endif()