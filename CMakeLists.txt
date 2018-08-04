cmake_minimum_required(VERSION 3.10.0)
set(CMAKE_CXX_STANDARD 17)

project(vcbld)

include(~/vcpkg/scripts/buildsystems/vcpkg.cmake)

add_subdirectory(src)

enable_testing()

add_subdirectory(tests)
