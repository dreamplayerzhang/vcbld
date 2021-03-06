set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/release)
SET(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> Scr <TARGET> <LINK_FLAGS> <OBJECTS>")
SET(CMAKE_CXX_ARCHIVE_FINISH "<CMAKE_RANLIB> -no_warning_for_no_symbols -c <TARGET>")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "-Weverything")

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
        # set(CMAKE_SHARED_LINKER_FLAGS -L/usr/local/opt/llvm/lib)
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

set(SOURCE_FILES "pch.cpp" "args.cpp" "builder.cpp" "help.cpp" "conf.cpp" "init.cpp" "gen.cpp" "pkg.cpp" "prep.cpp")

add_library(vcbldlib STATIC ${SOURCE_FILES})
add_executable(${PROJECT_NAME} vcbld.cpp)

target_include_directories(vcbldlib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

target_link_libraries(${PROJECT_NAME} PRIVATE vcbldlib)

if(GCC)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
elseif(CLANG)
    target_link_libraries(${PROJECT_NAME} PRIVATE c++experimental)
endif()

install(TARGETS ${PROJECT_NAME} 
        CONFIGURATIONS Release
        RUNTIME DESTINATION bin)
