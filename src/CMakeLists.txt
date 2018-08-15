set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "-Weverything")

if(CMAKE_COMPILER_IS_GNUXX OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    set(GCC 1)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "AppleClang")
        message(FATAL_ERROR
"Building the vcbld requires support for the C++ Filesystem TS.
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

set(SOURCE_FILES "main.cpp" "args.cpp" "builder.cpp" "help.cpp" "conf.cpp" "init.cpp" "gen.cpp")

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include)

if(GCC)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++fs)
elseif(CLANG)
    target_link_libraries(${PROJECT_NAME} PRIVATE c++experimental)
endif()

install(
	TARGETS ${PROJECT_NAME}
	CONFIGURATIONS Release
    RUNTIME DESTINATION bin
)

if(WIN32)
include(CPack REQUIRED)
include(CPackIFW REQUIRED)

cpack_add_component(${PROJECT_NAME}.exe
    DISPLAY_NAME "vcbld"
    DESCRIPTION "A build system that works with vcpkg")
cpack_ifw_configure_component(${PROJECT_NAME}.exe
    VERSION "0.1.0")
cpack_ifw_add_repository(myrepo
    URL "https://github.com/MoAlyousef/vcbld"
	DISPLAY_NAME "vcbld repository.")
endif()