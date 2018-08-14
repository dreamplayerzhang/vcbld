set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${PROJECT_BINARY_DIR}/bin/debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${PROJECT_BINARY_DIR}/bin/release)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# set(CMAKE_CXX_FLAGS "-Weverything")

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
	message(FATAL_ERROR "Prevented in-tree built. Please create a build directory outside of the source code and call cmake from there. Thank you.")
endif()

find_package(Boost COMPONENTS filesystem system REQUIRED)

add_executable(${PROJECT_NAME} "main.cpp" "args.cpp" "builder.cpp" "help.cpp" "conf.cpp" "init.cpp" "gen.cpp")

target_include_directories(${PROJECT_NAME} PUBLIC ${VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include)
target_link_libraries(${PROJECT_NAME} PUBLIC Boost::filesystem Boost::system)
