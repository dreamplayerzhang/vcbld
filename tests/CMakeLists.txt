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

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/catch)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)

set(TEST_FILES test.cpp initTest.cpp)

add_executable(mainTest ${TEST_FILES})
target_link_libraries(mainTest PRIVATE vcbldlib)

if(GCC)
    target_link_libraries(mainTest PRIVATE stdc++fs)
elseif(CLANG)
    target_link_libraries(mainTest PRIVATE c++experimental)
endif()

add_test(Tests mainTest)

